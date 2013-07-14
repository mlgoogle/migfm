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

namespace mig_lbs {

class BaiduLBSConnector {
public:
	BaiduLBSConnector();
	~BaiduLBSConnector();

public:
	int SetPOI(int64 user_id, double longitude, double latitude,
			const std::string &data);

	int DelPOI(int64 user_id);

	int SearchNearby(double longitude, double latitude, uint32 radius,
			const std::string &filter);

	int SearchRegin(double longitude, double latitude);

	int SearchBounds(double longitude1, double latitude1,
			double longitude2, double latitude2);

	int SearchDetail();

private:
	int _CreatePOI(int64 user_id, double longitude, double latitude, int64 &poi_id);
	int _CreatePOIEX(int64 user_id, int64 poi_id, const std::string& data);
	int _UpdatePOI(int64 poi_id, double longitude, double latitude);
	int _UpdatePOIEX(int64 poi_id, const std::string& data);
	int _DelPOI(int64 poi_id);
	int _DelPOIEX(int64 poi_id);
	int _SearchNearby(double longitude, double latitude, uint32 radius,
			const std::string &filter);

	std::string CalcSN(const std::string &base_string);

	std::string UrlEncode(const std::string &str);
	std::string UrlDecode(const std::string &str);
};

} // namespace mig_lbs

#endif /* _BAIDU_LBS_CONNECTOR_H_ */
