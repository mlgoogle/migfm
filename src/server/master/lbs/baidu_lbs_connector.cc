/*
 * baidu_lbs_connector.cpp
 *
 *  Created on: 2013-7-6
 *      Author: huaiyu
 */

#include "baidu_lbs_connector.h"

#include <sstream>
#include "http_response.h"
#include "log/mig_log.h"
#include "json/json.h"
#include "redis_connector.h"
//#include "curl/curl.h"
//#include "md5.h"

namespace mig_lbs {

const std::string BD_ACCESS_KEY = "A63c7ee45648535fa6144615846e7b13";

const std::string BD_DATABOX_ID = "13699";
const std::string BD_URL_POI = "http://api.map.baidu.com/geodata/poi";
const std::string BD_URL_POIEX = "http://api.map.baidu.com/geodata/poiext";
const std::string BD_URL_SEARCH = "http://api.map.baidu.com/geosearch/poi";

BaiduLBSConnector::BaiduLBSConnector() {
	// TODO Auto-generated constructor stub

}

BaiduLBSConnector::~BaiduLBSConnector() {
	// TODO Auto-generated destructor stub
}

int BaiduLBSConnector::SetPOI(int64 user_id, double longitude,
		double latitude, const std::string& data) {
	using namespace std;

	if (0 == user_id)
		return -1;

	RedisConnector *rc = RedisConnector::GetInstance();

	int64 poi_id = rc->FindUserPOIID(user_id);

	if (0 == poi_id) {
		// 不存在POI,创建
		if (0 != _CreatePOI(user_id, longitude, latitude, poi_id)) {
			return -1;
		}

		ASSERT(poi_id != 0);
		_CreatePOIEX(user_id, poi_id, data);

		rc->BindUserPOI(user_id, poi_id);
	} else {
		// 已存在,更新
		_UpdatePOI(poi_id, longitude, latitude);

		_UpdatePOIEX(poi_id, data);
	}

	return 0;
}

int BaiduLBSConnector::DelPOI(int64 user_id) {
	using namespace std;

	if (0 == user_id)
		return -1;

	RedisConnector *rc = RedisConnector::GetInstance();

	int64 poi_id = rc->FindUserPOIID(user_id);

	if (0 != poi_id) {
		_DelPOIEX(poi_id);
		_DelPOI(poi_id);
	}

	rc->DeleteUserPOI(user_id);

	return 0;
}

//std::string BaiduLBSConnector::CalcSN(const std::string& base_string) {
//	std::string str1 = base_string + BD_ACCESS_KEY;
//	std::string str2 = UrlEncode(str1);
//
//	return MD5String(str2);
//}
//
//std::string BaiduLBSConnector::UrlEncode(const std::string& str) {
//	std::string ret;
//
//	CURL *handle = curl_easy_init();
//	if (NULL == HANDLE)
//		return ret;
//
//	char *result = curl_easy_escape(handle, str.c_str(), str.length());
//	if (NULL == result)
//		ret.assign(result);
//
//	curl_free(result);
//	curl_easy_cleanup(handle);
//
//	return ret;
//}
//
//std::string BaiduLBSConnector::UrlDecode(const std::string& str) {
//	std::string ret;
//
//	CURL *handle = curl_easy_init();
//	if (NULL == HANDLE)
//		return ret;
//
//	int out_len = 0;
//	char *result = curl_easy_unescape(handle, str.c_str(), str.length(), &out_len);
//	if (NULL == result)
//		ret.assign(result);
//
//	curl_free(result);
//	curl_easy_cleanup(handle);
//
//	return ret;
//}

int BaiduLBSConnector::_CreatePOI(int64 user_id, double longitude,
									double latitude, int64& poi_id) {
	using namespace std;

	ASSERT(user_id != 0);

	poi_id = 0;

	HttpPost post(BD_URL_POI);
	stringstream ss;
	ss << "method=create";
	ss << "&name=" << "migfm_user";
	ss << "&original_lat=" << latitude;
	ss << "&original_lon=" << longitude;
	ss << "&original_coord_type=" << 1;
	ss << "&databox_id=" << BD_DATABOX_ID;
	ss << "&ak=" << BD_ACCESS_KEY;
	ss << ends;
	string str_post = ss.str();
	MIG_INFO(USER_LEVEL, "lbs poi create request:%s", str_post.c_str());
	if (post.Post(str_post.c_str())) {
		MIG_WARN(USER_LEVEL, "lbs poi create failed: calling faild");
		return -1;
	}

	string content;
	post.GetContent(content);
	MIG_INFO(USER_LEVEL, "lbs poi create response:%s", content.c_str());

	Json::Reader jrd;
	Json::Value val;

	if (!jrd.parse(content, val)) {
		MIG_WARN(USER_LEVEL, "lbs poi create failed: invalid json format");
		return -2;
	}

	int status = val["status"].asInt();
	if (0 != status) {
		MIG_WARN(USER_LEVEL, "lbs poi create failed! code:%d, msg:%s", status,
				val["message"].asCString());
		return status;
	}

	poi_id = val["id"].asInt();

	return 0;
}

int BaiduLBSConnector::_UpdatePOI(int64 poi_id, double longitude, double latitude) {
	ASSERT(poi_id != 0);

	using namespace std;

	stringstream url;
	url << BD_URL_POI << "/" << poi_id << ends;
	string str_url = url.str();
	MIG_INFO(USER_LEVEL, "lbs poi update url:%s", str_url.c_str());

	HttpPost http_post(str_url);

	stringstream post;
	post << "method=update";
	post << "&original_lat=" << latitude;
	post << "&original_lon=" << longitude;
	post << "&original_coord_type=" << 1;
	post << "&ak=" << BD_ACCESS_KEY;
	post << ends;

	string str_post = post.str();
	MIG_INFO(USER_LEVEL, "lbs poi update request:%s", str_post.c_str());
	if (!http_post.Post(str_post.c_str())) {
		MIG_WARN(USER_LEVEL, "lbs poi update failed: calling failed");
		return -1;
	}

	string content;
	http_post.GetContent(content);
	MIG_INFO(USER_LEVEL, "lbs poi update response:%s", content.c_str());

	Json::Reader jrd;
	Json::Value val;

	if (!jrd.parse(content, val)) {
		MIG_WARN(USER_LEVEL, "lbs poi update failed: invalid json format");
		return -2;
	}

	int status = val["status"].asInt();
	if (0 != status) {
		MIG_WARN(USER_LEVEL, "lbs poi update failed! code:%d, msg:%s", status,
				val["message"].asCString());
		return status;
	}

	return 0;
}

int BaiduLBSConnector::_CreatePOIEX(int64 user_id, int64 poi_id, const std::string& data) {
	using namespace std;

	ASSERT(user_id != 0);
	ASSERT(poi_id != 0);

	HttpPost post(BD_URL_POIEX);
	stringstream ss;
	ss << "method=create";
	ss << "&poi_id=" << poi_id;
	ss << "&user_id=" << user_id;
	ss << "&extra_data=" << data;
	ss << "&ak=" << BD_ACCESS_KEY;
	ss << ends;
	string str_post = ss.str();
	MIG_INFO(USER_LEVEL, "lbs poiex create request:%s", str_post.c_str());
	if (!post.Post(str_post.c_str())) {
		MIG_WARN(USER_LEVEL, "lbs poiex create failed: call failed");
		return -1;
	}

	string content;
	post.GetContent(content);
	MIG_INFO(USER_LEVEL, "lbs poiex create response:%s", content.c_str());

	Json::Reader jrd;
	Json::Value val;

	if (!jrd.parse(content, val)) {
		MIG_WARN(USER_LEVEL, "lbs poiex create failed: invalid json format");
		return -2;
	}

	int status = val["status"].asInt();
	if (0 != status) {
		MIG_WARN(USER_LEVEL, "lbs poiex create failed! code:%d, msg:%s", status,
				val["message"].asCString());
		return status;
	}

	return 0;
}

int BaiduLBSConnector::_UpdatePOIEX(int64 poi_id, const std::string& data) {
	using namespace std;

	ASSERT(poi_id != 0);

	HttpPost http_post(BD_URL_POIEX);
	stringstream post;
	post << "method=update";
	post << "&poi_id=" << poi_id;
	post << "&extra_data=" << data;
	post << "&ak=" << BD_ACCESS_KEY;
	post << ends;
	string str_post = post.str();
	MIG_INFO(USER_LEVEL, "lbs poiex update request:%s", str_post.c_str());
	if (!http_post.Post(str_post.c_str())) {
		MIG_WARN(USER_LEVEL, "lbs poiex update failed: calling failed");
		return -1;
	}

	string content;
	http_post.GetContent(content);
	MIG_INFO(USER_LEVEL, "lbs poiex update response:%s", content.c_str());

	Json::Reader jrd;
	Json::Value val;

	if (!jrd.parse(content, val)) {
		MIG_WARN(USER_LEVEL, "lbs poiex update failed: invalid json format");
		return -2;
	}

	int status = val["status"].asInt();
	if (0 != status) {
		MIG_WARN(USER_LEVEL, "lbs poiex update failed! code:%d, msg:%s", status,
				val["message"].asCString());
		return status;
	}

	return 0;
}

int BaiduLBSConnector::_DelPOI(int64 poi_id) {
	ASSERT(poi_id != 0);

	using namespace std;

	stringstream url;
	url << BD_URL_POI << "/" << poi_id << ends;
	string str_url = url.str();
	MIG_INFO(USER_LEVEL, "lbs poi delete url:%s", str_url.c_str());

	HttpPost http_post(str_url);

	stringstream post;
	post << "method=delete";
	post << "&ak=" << BD_ACCESS_KEY;
	post << ends;

	string str_post = post.str();
	MIG_INFO(USER_LEVEL, "lbs poi delete request:%s", str_post.c_str());
	if (!http_post.Post(str_post.c_str())) {
		MIG_WARN(USER_LEVEL, "lbs poi delete failed: calling failed");
		return -1;
	}

	string content;
	http_post.GetContent(content);
	MIG_INFO(USER_LEVEL, "lbs poi delete response:%s", content.c_str());

	Json::Reader jrd;
	Json::Value val;

	if (!jrd.parse(content, val)) {
		MIG_WARN(USER_LEVEL, "lbs poi delete failed: invalid json format");
		return -2;
	}

	int status = val["status"].asInt();
	if (0 != status) {
		MIG_WARN(USER_LEVEL, "lbs poi delete failed! code:%d, msg:%s", status,
				val["message"].asCString());
		return status;
	}

	return 0;
}

int BaiduLBSConnector::SearchNearby(double longitude, double latitude,
		uint32 radius, const std::string& filter) {

	_SearchNearby(longitude, latitude, radius, filter);

	return 0;
}

int BaiduLBSConnector::_DelPOIEX(int64 poi_id) {
	using namespace std;

	ASSERT(poi_id != 0);

	HttpPost http_post(BD_URL_POIEX);
	stringstream post;
	post << "method=delete";
	post << "&poi_id=" << poi_id;
	post << "&ak=" << BD_ACCESS_KEY;
	post << ends;
	string str_post = post.str();
	MIG_INFO(USER_LEVEL, "lbs poiex delete request:%s", str_post.c_str());
	if (!http_post.Post(str_post.c_str())) {
		MIG_WARN(USER_LEVEL, "lbs poiex delete failed: calling failed");
		return -1;
	}

	string content;
	http_post.GetContent(content);
	MIG_INFO(USER_LEVEL, "lbs poiex delete response:%s", content.c_str());

	Json::Reader jrd;
	Json::Value val;

	if (!jrd.parse(content, val)) {
		MIG_WARN(USER_LEVEL, "lbs poiex delete failed: invalid json format");
		return -2;
	}

	int status = val["status"].asInt();
	if (0 != status) {
		MIG_WARN(USER_LEVEL, "lbs poiex delete failed! code:%d, msg:%s", status,
				val["message"].asCString());
		return status;
	}

	return 0;
}

int BaiduLBSConnector::_SearchNearby(double longitude, double latitude,
		uint32 radius, const std::string& filter) {
	using namespace std;

	stringstream get;
	get << BD_URL_SEARCH;
	get << "?filter=" << "databox:" << BD_DATABOX_ID;
	if (!filter.empty())
		get << "|" << filter; // FIXME: 检测filter合法性
	get << "&location=" << latitude << "," << longitude;
	get << "&radius=" << radius;
	get << "&scope=" << 2;
	get << "&ak=" << BD_ACCESS_KEY;
	get << ends;
	string str_get = get.str();
	MIG_INFO(USER_LEVEL, "lbs search nearby request:%s", str_get.c_str());

	HttpResponse rsp(str_get);
	if (!rsp.Get()) {
		MIG_WARN(USER_LEVEL, "lbs search nearby failed: calling failed");
		return -1;
	}

	string content;
	rsp.GetContent(content);
	MIG_INFO(USER_LEVEL, "lbs search nearby response:%s", content.c_str());

	Json::Reader jrd;
	Json::Value val;

	if (!jrd.parse(content, val)) {
		MIG_WARN(USER_LEVEL, "lbs search nearby failed: invalid json format");
		return -2;
	}

	int status = val["status"].asInt();
	if (0 != status) {
		MIG_WARN(USER_LEVEL, "lbs search nearby failed! code:%d, msg:%s", status,
				val["message"].asCString());
		return status;
	}

	return 0;
}

} // namespace mig_lbs
