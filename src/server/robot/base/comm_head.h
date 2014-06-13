#ifndef __MIG_ROBOT_COMM_HEAD_H__
#define __MIG_ROBOT_COMM_HEAD_H__
#include "basic/basictypes.h"
#include <list>
#include <string>
#define  TOKEN_LEN 32
#define  NICKNAME_LEN 48
#define  HEAD_URL_LEN 64

enum operatorcode
{
	HEART_PACKET = 100,//与机器人客户端保持心跳连接，如果接收不到收回改机器人
	NOTICE_USER_LOGIN = 1000,//通知客户端用户登录，并保存两个机器人个人信息

};

enum msgtype
{
	ERROR_TYPE = 0,
	USER_TYPE = 1,
	CHAT_TYPE = 2,
	MEDIA_TYPE = 3,
};

struct PacketHead{
   int32 packet_length;
   int32 operate_code;
   int32 data_length;
   int32 current_time;
   int16 msg_type;
   int8  is_zip;
   int64 msg_id;
   int32 reserverd;
}; //31
#define PACKET_HEAD_LENGTH (sizeof(int32) * 5 + sizeof(int16) + sizeof(int8) + sizeof(int64))


#define ROBOTINFO_SIZE (sizeof(int64) * 2 + sizeof(int32) * 2 + NICKNAME_LEN)
struct RobotInfo{
	int64 uid;
	int64  songid;
	double latitude;
	double longitude;
	char nickname[NICKNAME_LEN];
};

struct NoticeUserLogin{
	int64 uid;
	std::list<struct RobotInfo*> robot_list;
};
#endif
