#ifndef __MIG_CHAT_COMM_HEAD_H__
#define __MIG_CHAT_COMM_HEAD_H__
#include "basic/basictypes.h"
#include <list>
#include <string>
#define  TOKEN_LEN 32
#define  NICKNAME_LEN 48
#define  HEAD_URL_LEN 64



enum errorcode{
	PLATFORM_ID_NOT_EXIST = 1,
	USER_ID_NOST_EXIST = 2
};

enum operatorcode
{
	PACKET_CONFIRM = 100,
	HEART_PACKET = 200,
	CHAT_ERROR = 300,
	USER_LOGIN = 1000,
	USER_LOGIN_SUCESS = 1001,
	USER_LOGIN_FAILED = 1002,
	USER_QUIT = 1010,
	USER_NOTIFICATION_QUIT = 1011,
	REQ_OPPOSITION_INFO = 1020,
	GET_OPPOSITION_INFO = 1021,
	TEXT_CHAT_PRIVATE_SEND = 1100,
	TEXT_CHAT_PRIVATE_RECV = 1101
};

enum msgtype
{
	ERROR_TYPE = 0,
	USER_TYPE = 1,
	CHAT_TYPE = 2,
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

//PACKET_CONFIRM = 100
#define PACKET_CONFIRM_SIZE (sizeof(int64) * 3 + TOKEN_LEN)
struct PacketConfirm:public PacketHead{
	int64 platform_id;
	int64 send_user_id;
	int64 recv_user_id;
	int64 session_id;
	char token[TOKEN_LEN];
};

//USER_LOGIN = 1000
#define USER_LOGIN_SIZE (sizeof(int64) * 2 + sizeof(int8) * 3 + TOKEN_LEN) //51
struct UserLogin:public PacketHead{
	int64 platform_id;
	int64 user_id;
	int8  net_type;
	int8  user_type;
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
#define REQ_OPPOSITION_INFO_SIZE (sizeof(int64) * 4 + sizeof(int16) + TOKEN_LEN)
struct ReqOppstionInfo : public PacketHead{
	int64 platform_id;
	int64 user_id;
	int64 oppostion_id;
	int16 type;
	char token[TOKEN_LEN];
};

#define OPPSITIONINFO_SIZE (sizeof(int64) + sizeof(64) + NICKNAME_LEN + HEAD_URL_LEN)
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
#define TEXTCHATPRIVATESEND_SIZE (sizeof(int64) * 3 + TOKEN_LEN + vTextChatPrivateSend->content.length())

struct TextChatPrivateSend:public PacketHead{
	int64 platform_id;
	int64 send_user_id;
	int64 recv_user_id;
	int64 session;
	char token[TOKEN_LEN];
	std::string content;
};

//TEXT_CHAT_PRIVATE_RECV = 1101
#define TEXTCHATPRIVATERECV_SIZE (sizeof(int64) * 2 + vTextChatPrivateRecv->content.length())
struct TextChatPrivateRecv:public PacketHead{
	int64 platform_id;
	int64 send_user_id;
	int64 recv_user_id;
	std::string content;
};
#endif
