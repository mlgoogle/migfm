#ifndef MIG_FM_PUBLIC_PROTOCOL_COMMUNICATION_RULES_H__
#define MIG_FM_PUBLIC_PROTOCOL_COMMUNICATION_RULES_H__

#include "basic/basictypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

namespace packet {

enum OperateCode{
    
  PACKET_LOAD_BALANCING_REQ	= 100,
  PACKET_LOAD_BALANCING_ACK = 101,
  PACKET_LOAD_BALANCING_ERROR = 102,
  PACKET_HEART_TICKET = 200,
  PACKET_USER_LOGIN_REQ = 1000,
  PACKET_USER_LOGIN_ACK = 1001,
  PACKET_USER_LOGIN_ERROR = 1002,
  PACKET_USER_PRIVATE_CHAT_REQ = 1300,
  PACKET_USER_PRIVATE_CHAT_ACK = 1301,
  PACKET_USER_CHAT_ERROR = 1302
};

#define  PACKET_HEAD_LENGTH 20
#define  TOKEN_LEN 33
#define  IP_LEN    33

struct PacketHead{
   int32 packet_length;
   int32 operate_code;
   int32 operate_result;
   int32 data_length;
   int32 reserved;
};

//PACKET_HEART_TICKET
struct HearTicket : public PacketHead{
	int64 seesion;
	char  token[TOKEN_LEN];
};

//PACKET_LOAD_BALANCING_REQ
#define LOAD_BALANCEINGREQ_SIZE (TOKEN_LEN - 1)
struct LoadBalancingREQ : public PacketHead{
    char token[TOKEN_LEN];
};

//PACKET_LOAD_BALANCING_ACK
#define LOAD_BALANCEINACK_SIZE (IP_LEN - 1)
struct LoadBalancingACK : public PacketHead{
	char ip[IP_LEN];
}

//PACKET_USER_LOGIN_ERROR  PACKET_USER_CHAT_ERROR
#define CHAT_ERR_SIZE (chat_err->msg.length())
struct ChatERR : public PacketHead{
	std::string msg;
};

//PACKET_USER_LOGIN_REQ
#define USER_LOGIN_REQ_SIZE (4 * 1 + 1 * 8 + TOKEN_LEN -1)
struct UserLoginREQ : public PacketHead{
	int8    type;
	int8    net_type;
	int8    source;
	int8    hide;
	int64   user_id;
	char    token[TOKEN_LEN];
};

//PACKET_USER_LOGIN_ACK
#define USER_LOGIN_ACK_SIZE (1 * 8 +TOKEN_LEN -1)
struct UserLoginACK : public PacketHead{
   int64  session;
   char  token[TOKEN_LEN];
};

//PACKET_USER_PRIVATE_CHAT_REQ
#define CHAT_PRIVATE_REQ_SIZE (3 * 8 + TOKEN_LEN - 1 + chat_private_req.msg.length())
struct ChatPrivateREQ : public PacketHead{
	int64        send_uid;
	int64        recv_uid;
	int64        session;
	char         token[TOKEN_LEN];
	std::string  msg;
};

//PACKET_USER_PRIVATE_CHAT_ACK
#define CHAT_PRIVATE_ACK_SIZE (2 * 8 + chat_private_ack.msg.length())
struct ChatPrivateACK : public PacketHead{
	int64          send_uid;
	int64          recv_uid;
	std::string    msg;
};


}
#endif