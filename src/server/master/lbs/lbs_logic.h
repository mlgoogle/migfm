/*
 * lbs_logic.h
 *
 *  Created on: 2013-7-15
 *      Author: huaiyu
 */

#ifndef _LBS_LOGIC_H_
#define _LBS_LOGIC_H_

#include <string>
#include "basic/basictypes.h"
#include "baidu_lbs_connector.h"
#include "redis_connector.h"
#include "plugins.h"
#include "basic/http_packet.h"

namespace Json {
	class Value;
}

namespace mig_lbs {

class LBSLogic {
public:
	static LBSLogic *GetInstance();
	static void FreeInstance();

public:
	bool Init();

	bool OnMsgRead(struct server *srv, int socket,
            const void *msg, int len);

public:
	bool OnMsgSetPoi(packet::HttpPacket& packet, Json::Value &result,
			int &status, std::string &msg);
	bool OnMsgSearchNearby(packet::HttpPacket& packet, Json::Value &result,
			int &status, std::string &msg);
	bool OnMsgUnknown(packet::HttpPacket& packet, Json::Value &result,
			int &status, std::string &msg);

public:
	int SetPOI(int64 user_id, double longitude, double latitude, const std::string &data,
			std::string &response, std::string &err_msg);

	int DelPOI(int64 user_id,
			std::string &response, std::string &err_msg);

	int SearchNearby(double longitude, double latitude, uint32 radius,const std::string &filter,
			uint32 page_index, uint32 page_size, Json::Value &result,
			std::string &response, std::string &err_msg);

	int SearchRegin(double longitude, double latitude,
			std::string &response, std::string &err_msg);

	int SearchBounds(double longitude1, double latitude1, double longitude2, double latitude2,
			std::string &response, std::string &err_msg);

	int SearchDetail(
			std::string &response, std::string &err_msg);

	int SendFull(int socket, const char *buffer, size_t nbytes);

public:
	~LBSLogic();

private:
	RedisConnector		redis_conn_;
	BaiduLBSConnector	bd_lbs_coon_;

private:
	LBSLogic();
	DISALLOW_COPY_AND_ASSIGN(LBSLogic);

private:
	static LBSLogic *inst_;
};

} /* namespace mig_lbs */
#endif /* _LBS_LOGIC_H_ */
