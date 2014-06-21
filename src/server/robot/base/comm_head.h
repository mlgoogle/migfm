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
	NOTICE_USER_ROBOT_LOGIN = 1000,//通知客户端用户登录，并发送两个机器人个人信息
	NOTICE_USER_ROBOT_HANDSEL_SONG = 1001,//通知机器人客户端赠送歌曲
	SCHEDULER_LOGIN = 2000,//机器人调度器登录
	ROBOT_LOGIN = 3000,//机器人登陆成功
	NOTICE_USER_LOGIN = 4000,//咪呦通知机器人服务器用户已经登录
	NOTICE_USER_DEFAULT_SONG = 4001,//咪呦通知机器人服务器用户听的歌

};

enum msgtype
{
	ERROR_TYPE = 0,
	USER_TYPE = 1,

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
//NOTICE_USER_ROBOT_LOGIN
struct NoticeRobotLogin:public PacketHead{
	int64 uid;
	std::list<struct RobotInfo*> robot_list;
};
//NOTICE_USER_LOGIN
#define NOTICEUSERLOGIN_SIZE (sizeof(int64) * 2 + sizeof(double) * 2)
struct NoticeUserLogin:public PacketHead{
	int64 platform_id;
	int64 uid;
	double latitude;
	double longitude;
};

//NOTICE_USER_DEFAULT_SONG
#define NOTICEUSERDEFAULTSONG_SIZE (sizeof(int64) * 3 + sizeof(int32) +vNoticeUserDefaultSong->mode.length())
struct NoticeUserDefaultSong: public PacketHead{
	int64 platform_id;
	int64 uid;
	int64 songid;
	int32 type_id;
	std::string mode;
};
//ROBOT_LOGIN
#define ROBOT_LOGIN_SIZE (sizeof(int64) * 3)
struct RobotLogin:public PacketHead{
	int64 platform_id;
	int64 uid;
	int64 robot_id;
};

//NOTICE_USER_ROBOT_HANDSEL_SONG
#define NOTICEUSERROBOTHANDSELSONG_SIZE  (sizeof(int64) * 4)
struct NoticeUserRobotHandselSong:public PacketHead{
	int64 platform_id;
	int64 uid;
	int64 robot_id;
	int64 song_id;
};


//SCHEDULER_LOGIN
#define SCHEDULER_LOGIN_SIZE (sizeof(int64) + vSchedulerLogin->machine_id.length())
struct SchedulerLogin:public PacketHead{
	int64 platform_id;
	std::string machine_id;
};
#endif
