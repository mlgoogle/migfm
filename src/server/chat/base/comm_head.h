#ifndef __MIG_CHAT_COMM_HEAD_H__
#define __MIG_CHAT_COMM_HEAD_H__
#include "basic/basictypes.h"
#include <list>
#include <string>
#define  TOKEN_LEN 32
#define  NICKNAME_LEN 48
#define  HEAD_URL_LEN 256



enum errorcode{
	PLATFORM_ID_NOT_EXIST = 1,
	USER_ID_NOST_EXIST = 2
};

enum operatorcode
{
	HEART_PACKET = 100,
	PACKET_CONFIRM = 200,
	CHAT_ERROR = 300,
	USER_LOGIN = 1000,
	USER_LOGIN_SUCESS = 1001,
	USER_LOGIN_FAILED = 1002,
	USER_QUIT = 1010,
	USER_NOTIFICATION_QUIT = 1011,
	REQ_OPPOSITION_INFO = 1020,
	GET_OPPOSITION_INFO = 1021,
	USER_ONLINE_REQ = 1030,//新用户进入群组成功
	USER_ONLINE_RSP = 1031,//通知所有用户新用户进入
	TEXT_CHAT_PRIVATE_SEND = 1100,
	TEXT_CHAT_PRIVATE_RECV = 1101,
	MULTI_SOUND_SEND = 2100,
	MULTI_SOUND_RECV = 2101,
	MULTI_CHAT_SEND = 2200,
	MULTI_CHAT_RECV = 2201
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

//PACKET_CONFIRM = 200
#define PACKET_CONFIRM_SIZE (sizeof(int64) * 5 + TOKEN_LEN)
struct PacketConfirm:public PacketHead{
	int64 platform_id;
	int64 send_user_id;
	int64 recv_user_id;
	int64 private_msg_id;
	int64 session_id;
	char token[TOKEN_LEN];
};

//USER_LOGIN = 1000
#define USER_LOGIN_SIZE (sizeof(int64) * 2 + sizeof(int8) * 3 + TOKEN_LEN) //51
struct UserLogin:public PacketHead{
	int64 platform_id;
	int64 user_id;
	int8  net_type;
	int8  user_type; //0,游客，1，用户 2，机器人
	int8  device;
	char  token[TOKEN_LEN];
};

//USER_LOGIN_SUCESS = 1001
#define USER_LOGIN_SUCESS_SIZE (sizeof(int64) * 3 + TOKEN_LEN + NICKNAME_LEN + HEAD_URL_LEN)
struct UserLoginSucess:public PacketHead
{
	int64 platform_id;
    int64 user_id;
	int64 nick_number;
	char token[TOKEN_LEN];
	char nickname[NICKNAME_LEN];
	char head_url[HEAD_URL_LEN];
};

//USER_LOGIN_FAILED = 1002
#define USER_LOGIN_FAILED_SIZE (sizeof(int64) + vChatFailed->error_msg.length())
struct ChatFailed:public PacketHead{
	int64 platform_id;
	std::string error_msg;
};

//USER_QUIT = 1010
#define USER_QUIT_SIZE (sizeof(int64) * 3 + TOKEN_LEN)
struct UserQuit:public PacketHead{
    int64 platform_id;
	int64 user_id;
	int64 session;
	char token[TOKEN_LEN];
};

//USER_NOTIFICATION_QUIT = 1011
#define USER_NOTIFICATION_QUIT_SIZE (sizeof(int64) * 2)
struct UserQuitNotification :public PacketHead{
	int64 platform_id;
	int64 user_id;
};

//REQ_OPPOSITION_INFO = 1020
#define REQ_OPPOSITION_INFO_SIZE (sizeof(int64) * 3 + sizeof(int16) + TOKEN_LEN)
struct ReqOppstionInfo : public PacketHead{
	int64 platform_id;
	int64 user_id;
	int64 oppostion_id;
	int16 type; //1 单聊 2 群聊 3 临时组
	char token[TOKEN_LEN];
};

#define OPPSITIONINFO_SIZE (sizeof(int64) * 2 + NICKNAME_LEN + HEAD_URL_LEN)
struct Oppinfo
{
	int64 user_id;
	int64 user_nicknumber;
	char nickname[NICKNAME_LEN];
	char user_head[HEAD_URL_LEN];

};

//GET_OPPOSITION_INFO = 1021

struct OppositionInfo:public PacketHead{
	int64 platform_id;
	int64 oppo_id;
	int64 oppo_nick_number;
	int64 session;
	int16 oppo_type;
	char  oppo_nickname[NICKNAME_LEN];
	char  oppo_user_head[HEAD_URL_LEN];
	std::list<struct Oppinfo*> opponfo_list;
};

//TEXT_CHAT_PRIVATE_SEND = 1100
#define TEXTCHATPRIVATESEND_SIZE (sizeof(int64) * 4 + TOKEN_LEN + vTextChatPrivateSend->content.length())

struct TextChatPrivateSend:public PacketHead{
	int64 platform_id;
	int64 send_user_id;
	int64 recv_user_id;
	int64 session;
	char token[TOKEN_LEN];
	std::string content;
};

//TEXT_CHAT_PRIVATE_RECV = 1101
#define TEXTCHATPRIVATERECV_SIZE (sizeof(int64) * 3 + vTextChatPrivateRecv->content.length())
struct TextChatPrivateRecv:public PacketHead{
	int64 platform_id;
	int64 send_user_id;
	int64 recv_user_id;
	std::string content;
};

//USER_ONLINE_REQ = 1030
#define USERONLINEREQ_SIZE (sizeof(int64) * 3 + TOKEN_LEN)
struct UserOnLineReq:public PacketHead{
	int64 platform_id;
	int64 group_id;
	int64 user_id;
	char token[TOKEN_LEN];
};

//USER_ONLINE_RSP = 1031
#define USERONLINERSP_SIZE (sizeof(int64) * 4 + NICKNAME_LEN + HEAD_URL_LEN)
struct UserOnLineRsp:public PacketHead{
	int64 platform_id;
	int64 group_id;
	int64 user_id;
	int64 user_nicknumber;
	char nickname[NICKNAME_LEN];
	char user_head[HEAD_URL_LEN];
};

//MULTI_SOUND_SEND = 2100
#define MULTISOUNDSEND_SIZE (sizeof(int64) * 4 + sizeof(int16) + TOKEN_LEN + vMultiSoundSend->sound_path.length())
struct MultiSoundSend:public PacketHead{
	int64 platform_id;
	int64 multi_id;// 由客户端产生，同一个类型进入相同的会话
	int16 multi_type;
	int64 send_user_id;
	int64 session;
	char token[TOKEN_LEN];
	std::string sound_path; //文件名: 讨论组id/发送者id/声音文件名(讨论组，发送者，当前时间拼装)
};

//MULTI_SOUND_RECV = 2101
#define MULTISOUNDRECV_SIZE (sizeof(int64) * 3 + vMultiSoundRecv->sound_path.length())
struct MultiSoundRecv:public PacketHead{
	int64 platform_id;
	int64 multi_id;
	int64 send_user_id;
	std::string sound_path;
};

//MULTI_CHAT_SEND = 2200,
#define MULTICHATSEND_SIZE (sizeof(int64) * 4 +TOKEN_LEN + vMultiChatSend->content.length())
struct MultiChatSend:public PacketHead{
	int64 platform_id;
	int64 multi_id;//群组号
	int64 send_user_id;
	int64 session;
	char  token[TOKEN_LEN];
	std::string content;
};

//MULTI_CHAT_RECV = 2201,
#define MULTICHATRECV_SIZE (sizeof(int64) * 3 + NICKNAME_LEN + vMultiChatRecv->content.length())
struct MultiChatRecv:public PacketHead{
	int64 platform_id;
	int64 multi_id;
	int64 send_user_id;
	char send_nickname[NICKNAME_LEN];
	std::string content;
};

#endif
