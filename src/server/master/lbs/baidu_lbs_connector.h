/*
 * baidu_lbs_connector.h
 *
 *  Created on: 2013-7-6
 *      Author: huaiyu
 */

#ifndef _BAIDU_LBS_CONNECTOR_H_
#define _BAIDU_LBS_CONNECTOR_H_

#include <string>
#include "basic/basictypes.h"

namespace Json {
	class Value;
}

namespace mig_lbs {


class BaiduLBSConnectorV2{
public:
	BaiduLBSConnectorV2();
	~BaiduLBSConnectorV2();
public:
	int CreatePOI(int64 user_id, double longitude,
		double latitude, int64& poi_id, std::string &response,
		std::string &err_msg);

	int UpdatePOI(int64 user_id, int64 poi_id, double longitude, double latitude,
		std::string &response, std::string &err_msg);

	int DelPOI(int64 poi_id,std::string &response, std::string &err_msg);

	int SearchNearby(double longitude, double latitude, uint32 radius,
		             const std::string &q,const std::string &filter,
					 uint32 page_index, uint32 page_size, Json::Value &result,
					 std::string &response, std::string &err_msg);
};

class BaiduLBSConnector {
public:
	BaiduLBSConnector();
	~BaiduLBSConnector();

public:
	int CreatePOI(int64 user_id, double longitude, double latitude, int64 &poi_id,
			std::string &response, std::string &err_msg);
	int CreatePOIEX(int64 user_id, int64 poi_id, const std::string& data,
			std::string &response, std::string &err_msg);
	int UpdatePOI(int64 poi_id, double longitude, double latitude,
			std::string &response, std::string &err_msg);
	int UpdatePOIEX(int64 poi_id, const std::string& data,
			std::string &response, std::string &err_msg);
	int DelPOI(int64 poi_id,
			std::string &response, std::string &err_msg);
	int DelPOIEX(int64 poi_id,
			std::string &response, std::string &err_msg);
	int SearchNearby(double longitude, double latitude, uint32 radius, 
		             const std::string &filter,
					 uint32 page_index, uint32 page_size, Json::Value &result,
					 std::string &response, std::string &err_msg);

	std::string CalcSN(const std::string &base_string);

	std::string UrlEncode(const std::string &str);
	std::string UrlDecode(const std::string &str);
};

} // namespace mig_lbs

#endif /* _BAIDU_LBS_CONNECTOR_H_ */
