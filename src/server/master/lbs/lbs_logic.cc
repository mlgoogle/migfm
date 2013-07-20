/*
 * lbs_logic.cc

 *
 *  Created on: 2013-7-15
 *      Author: huaiyu
 */
#include <string>
#include <vector>
#include "lbs_logic.h"
#include "json/json.h"
#include "log/mig_log.h"
#include "config/config.h"
#include "logic_comm.h"
#include "dic_comm.h"

namespace {
int SplitStringChr( const char *str, const char *char_set,
				   std::vector<std::string> &out )
{
	using std::string;

	assert(str != NULL);
	assert(char_set != NULL);

	out.clear();

	if (0 == char_set[0]) {
		if (str[0])
			out.push_back(str);
		return (int)out.size();
	}

	const char *find_ptr = NULL;
	str = str + ::strspn(str, char_set);
	while (str && (find_ptr=::strpbrk(str, char_set))) {
		if (str != find_ptr)
			out.push_back(string(str, find_ptr));
		str = str + ::strspn(find_ptr, char_set);
	}
	if (str && str[0])
		out.push_back(str);

	return (int)out.size();
}
}

namespace mig_lbs {

LBSLogic *LBSLogic::inst_ = NULL;

LBSLogic* LBSLogic::GetInstance() {
	if (NULL == inst_)
		inst_ = new LBSLogic();
	return inst_;
}

void LBSLogic::FreeInstance() {
	delete inst_;
}

int LBSLogic::SetPOI(int64 user_id, double longitude, double latitude,
		const std::string& data, std::string& response, std::string& err_msg) {
	using namespace std;

	if (0 == user_id)
		return -1;

	int64 poi_id = redis_conn_.FindUserPOIID(user_id);

	if (0 == poi_id) {
		// 不存在POI,创建
		if (0 != bd_lbs_coon_.CreatePOI(user_id, longitude, latitude, poi_id, response, err_msg)) {
			return -1;
		}

		ASSERT(poi_id != 0);
		bd_lbs_coon_.CreatePOIEX(user_id, poi_id, data, response, err_msg);

		redis_conn_.BindUserPOI(user_id, poi_id);
	} else {
		// 已存在,更新
		bd_lbs_coon_.UpdatePOI(poi_id, longitude, latitude, response, err_msg);

		bd_lbs_coon_.UpdatePOIEX(poi_id, data, response, err_msg);
	}

	return 0;
}

int LBSLogic::DelPOI(int64 user_id, std::string& response,
		std::string& err_msg) {
	using namespace std;

	if (0 == user_id)
		return -1;

	int64 poi_id = redis_conn_.FindUserPOIID(user_id);

	if (0 != poi_id) {
		//bd_lbs_coon_.DelPOIEX(poi_id, response, err_msg);
		bd_lbs_coon_.DelPOI(poi_id, response, err_msg);
	}

	redis_conn_.DeleteUserPOI(user_id);

	return 0;
}

int LBSLogic::SearchNearby(double longitude, double latitude, uint32 radius,
		const std::string& filter, uint32 page_index, uint32 page_size, Json::Value &result,
		std::string& response, std::string& err_msg) {

	bd_lbs_coon_.SearchNearby(longitude, latitude, radius, filter,
			page_index, page_size, result, response, err_msg);

	return 0;
}

bool LBSLogic::Init() {

}

bool LBSLogic::OnMsgRead(struct server* srv, int socket, const void* msg, int len) {
    const char* packet_stream = (const char*)(msg);
	packet::HttpPacket packet(packet_stream, len-1);
	std::string type;
	packet.GetPacketType(type);

	Json::Value root(Json::objectValue);
	int ret_status;
	std::string ret_msg;
	if (type=="setmypos"){
		OnMsgSetPoi(packet, root, ret_status, ret_msg);
	} else if (type=="searchnearby"){
		OnMsgSearchNearby(packet, root, ret_status, ret_msg);
	} else {
		OnMsgUnknown(packet, root, ret_status, ret_msg);
	}

	root["status"] = ret_status;
	if (ret_status != 1)
		root["msg"] = ret_msg;

	Json::FastWriter wr;
	std::string res = wr.write(root);
	SendFull(socket, res.c_str(), res.length());

	MIG_DEBUG(USER_LEVEL, "lbs request:%s, response:%s", type.c_str(), res.c_str());

    return true;
}

bool LBSLogic::OnMsgSetPoi(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg) {
	status = 0;
	msg.clear();

	std::string uid_str, location_str;
	if (!packet.GetAttrib("uid", uid_str)) {
		msg = "uid未指定";
		return false;
	}
	if (!packet.GetAttrib("location", location_str)) {
		msg = "location未指定";
		return false;
	}

	std::vector<std::string> location_pair;
	if (2 != SplitStringChr(location_str.c_str(), ",", location_pair)) {
		msg = "location参数格式错误";
		return false;
	}

	double latitude = atof(location_pair[0].c_str());
	double longitude = atof(location_pair[1].c_str());

	int64 uid = atoll(uid_str.c_str());
	std::string response;
	if (0 != SetPOI(uid, longitude, latitude, "", response, msg)) {
		return false;
	}

	status = 1;
	return true;
}

bool LBSLogic::OnMsgSearchNearby(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg) {
	status = 0;
	msg.clear();

	std::string uid_str, location_str, radius_str, page_index_str, page_size_str;
	if (!packet.GetAttrib("uid", uid_str)) {
		msg = "uid未指定";
		return false;
	}
	if (!packet.GetAttrib("location", location_str)) {
		msg = "location未指定";
		return false;
	}
	if (!packet.GetAttrib("radius", radius_str)) {
		radius_str = "1000";
	}
	if (!packet.GetAttrib("page_index", page_index_str)) {
		page_index_str = "0";
	}
	if (!packet.GetAttrib("page_size", page_size_str)) {
		page_size_str = "10";
	}

	std::vector<std::string> location_pair;
	if (2 != SplitStringChr(location_str.c_str(), ",", location_pair)) {
		msg = "location参数格式错误";
		return false;
	}

	double latitude = atof(location_pair[0].c_str());
	double longitude = atof(location_pair[1].c_str());
	uint32 radius = atoi(radius_str.c_str());
	int page_index = atoi(page_index_str.c_str());
	int page_size = atoi(page_size_str.c_str());

	int64 uid = atoll(uid_str.c_str());
	std::string response;
	Json::Value content;
	if (0 != SearchNearby(longitude, latitude, radius, "", page_index, page_size,
			content, response, msg)) {
		return false;
	}

	Json::Value &users = result["result"]["nearUser"];
	for (Json::Value::iterator it = content.begin();
		it != content.end();
		++it) {
		const Json::Value &item = *it;
		Json::Value val;
		val["userid"] = item["ext"]["user_id"];
		val["latitude"] = item["latitude"];
		val["longitude"] = item["longitude"];
		users.append(val);
	}

	status = 1;
	return true;
}

bool LBSLogic::OnMsgUnknown(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg) {
	using namespace Json;

	status = 0;
	msg = "不支持的接口";

	return true;
}

LBSLogic::LBSLogic() {
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL)
		return;

	r = config->LoadConfig(path);
	//storage::DBComm::Init(config->mysql_db_list_);
	//storage::MemComm::Init(config->mem_list_);
	storage::RedisComm::Init(config->redis_list_);
}

LBSLogic::~LBSLogic() {
	ThreadKey::DeinitThreadKey ();
}

int LBSLogic::SendFull(int socket, const char *buffer, size_t nbytes){
	ssize_t amt = 0;
	ssize_t total = 0;
	const char *buf = buffer;

	do {
		amt = nbytes;
		amt = send (socket, buf, amt, 0);
		buf = buf + amt;
		nbytes -= amt;
		total += amt;
	} while (amt != -1 && nbytes > 0);

	return (int)(amt == -1 ? amt : total);
}

} /* namespace mig_lbs */
