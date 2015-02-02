/*
 * logic_unit.h
 *
 *  Created on: 2014年12月2日
 *      Author: kerry
 */
#ifndef _BUDDHA_LOGIC_UNIT_H__
#define _BUDDHA_LOGIC_UNIT_H__

#include "logic/logic_infos.h"
#include "net/comm_head.h"

#define UNKONW_DIMENSIONS    "未知维度"
#define MOOD_NAME            "心情"
#define SCENE_NAME           "场景"
#define CHANNEL_NAME         "频道"
#define PLATFORM_NAME        "miyo"

#define UNKONW_DIMENSIONS_ID 100000

#define MUSICINFO_MAP         std::map<int64,base_logic::MusicInfo>
#define MUSICINFONLIST_MAP    std::map<int64,std::map<int64,base_logic::MusicInfo> >

namespace base_logic{

class LogicUnit{
public:
	static bool IPToGeocoderAndAddress(netcomm_recv::HeadPacket* packet,
			base_logic::LBSInfos* lbs_infos);

	static base_logic::LBSInfos*  GetGeocderAndAddress(netcomm_recv::HeadPacket* packet);

	static bool GetGeocoderWeather(base_logic::LBSInfos* lbsinfo,std::string& weather,std::string& temp);

	static void CreateToken(const int64 uid,std::string& token);

	static void SendMessage(const int socket,netcomm_send::HeadPacket* packet);

	static void SendErrorMsg(const int32 error_code,const int socket);
};

}

#define send_error      base_logic::LogicUnit::SendErrorMsg
#define send_message    base_logic::LogicUnit::SendMessage

#endif

