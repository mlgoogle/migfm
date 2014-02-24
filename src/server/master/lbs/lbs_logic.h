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

	bool OnMsgNearCollect(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg);

	bool OnMsgNearMusic(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg);

	bool OnMsgMusicFri(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg);

	bool OnMsgSearchNearbyV2(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg);

	bool OnMsgSameMusic(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg);


public:

	//fix me 宏定义 或者模板优化 
	 bool OnMsgPublicLbs(packet::HttpPacket& packet, Json::Value &result,
		int &status, std::string &msg,int flag);  
	/*
	flag 1 获取周围的用户               OnMsgSearchNearbyV2
	     2 获取周围在听你红心歌曲的用户 OnMsgSameMusic
		 3 获取周围总数                 OnMsgMusicFri
		 4 获取周围音乐                 OnMsgNearMusic
		 5 获取周围音乐个数及附近人个数 OnMsgNearCollect
	*/ 

	bool UpdateUserInfoPoi(int64 user_id,double longitude, 
		                 double latitude,std::string &msg);

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

	int SearchDetail(std::string &response, std::string &err_msg);

	bool GetUserCurrentMusic(const std::string& content,Json::Value& item,
		                    bool& is_user_like,
							std::map<std::string,std::string>* collect_musices=NULL,
							bool is_collect = false);

	bool GetMusicHotCltCmt(const std::string &songid,std::string &hot_num, 
		                   std::string &cmt_num, 
		                   std::string &clt_num);

	int GetMsgCount(const std::string &uid);

public:
	~LBSLogic();

private:
	RedisConnector		redis_conn_;
	BaiduLBSConnectorV2	bd_lbs_coon_;

private:
	LBSLogic();
	DISALLOW_COPY_AND_ASSIGN(LBSLogic);

private:
	static LBSLogic *inst_;
};

} /* namespace mig_lbs */
#endif /* _LBS_LOGIC_H_ */
