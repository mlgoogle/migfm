#include "protocol/cmd_packet.h"
#include "protocol/communication_rules.h"

namespace packet{

#define BULDHEAD()                   \
	out.Write32(packet_length);      \
	out.Write32(operate_code);       \
	out.Write32(operate_result);     \
	out.Write32(data_length);        \
	out.Write32(reserved)


bool ChatPack::PacketStream(const struct PacketHead* packet_head, 
							void **packet_stream,
							int32 *packet_stream_length){
	bool r = true;
	int32 packet_length = packet_head->packet_length;
	int32 operate_code = packet_head->operate_code;
	int32 operate_result = packet_head->operate_result;
	int32 data_length = packet_head->data_length;
	int32 reserved = packet_head->reserved;
	char *packet = NULL;
	char *data = NULL;

	switch (operate_code){
		case PACKET_LOAD_BALANCING_REQ:
			{
				struct LoadBalancingREQ* lb_req = (struct LoadBalancingREQ*)packet_head;
				data_length = LOAD_BALANCEINGREQ_SIZE;
				packet_length = PACKET_HEAD_LENGTH + data_length;
				packet = new char[packet_length];
				DataOutPacket out(false,packet_length);
				BULDHEAD();
				out.WriteData(lb_req->token,TOKEN_LEN - 1);
				packet = (char*) out.GetData();
			}
			break;

		case PACKET_LOAD_BALANCING_ACK:
			{
				struct LoadBalancingACK* lb_ack = (struct LoadBalancingACK*)packet_head;
				data_length = LOAD_BALANCEINACK_SIZE;
				packet_length = PACKET_HEAD_LENGTH + data_length;
				packet = new char[packet_length];
				DataOutPacket out(false,packet_length);
				BULDHEAD();
				out.WriteData(lb_ack->ip,IP_LEN - 1);
				packet = (char*) out.GetData();
			}
		    break;

		case PACKET_USER_LOGIN_REQ:
			{
				struct UserLoginREQ* user_login_req = (struct UserLoginREQ*)packet_head;
				data_length = USER_LOGIN_REQ_SIZE;
				packet_length = PACKET_HEAD_LENGTH + data_length;
				packet = new char [packet_length];
				DataOutPacket out(false,packet_length);
				BULDHEAD();
				out.Write8(user_login_req->type);
				out.Write8(user_login_req->net_type);
				out.Write8(user_login_req->source);
				out.Write8(user_login_req->hide);
				out.Write64(user_login_req->user_id);
				out.WriteData(user_login_req->token,TOKEN_LEN - 1);
				packet = (char*) out.GetData();
			}
			break;

		case PACKET_USER_LOGIN_ACK:
			{
				struct UserLoginACK* user_login_ack = (struct UserLoginACK*)packet_head;
				data_length = USER_LOGIN_ACK_SIZE;
				packet_length = PACKET_HEAD_LENGTH + data_length;
				packet = new char [packet_length];
				DataOutPacket out(false,packet_length);
				BULDHEAD();
				out.Write64(user_login_ack->session);
				out.WriteData(user_login_ack->token,TOKEN_LEN -1);
				packet = (char*) out.GetData();
			}
			break;

		case PACKET_USER_PRIVATE_CHAT_REQ:
			{
				struct ChatPrivateREQ* chat_private_req 
					= (struct ChatPrivateREQ*)packet_head;
				data_length = CHAT_PRIVATE_REQ_SIZE;
				packet_length =PACKET_HEAD_LENGTH + data_length;
				packet = new char[packet_length];
				DataOutPacket out(false,packet_length);
				BULDHEAD();
				out.Write64(chat_private_req->send_uid);
				out.Write64(chat_private_req->recv_uid);
				out.Write64(chat_private_req->session);
				out.WriteData(chat_private_req->token,TOKEN_LEN - 1);
				out.WriteData(chat_private_req->msg.c_str(),chat_private_req->msg.length());
				packet = (char*) out.GetData();
			}
			break;

		case PACKET_USER_PRIVATE_CHAT_ACK:
			{
				struct ChatPrivateACK* chat_private_ack 
					= (struct ChatPrivateREQ*)packet_head;
				data_length = CHAT_PRIVATE_ACK_SIZE;
				packet_length =PACKET_HEAD_LENGTH + data_length;
				packet = new char[packet_length];
				DataOutPacket out(false,packet_length);
				BULDHEAD();
				out.Write64(chat_private_ack->send_uid);
				out.Write64(chat_private_ack->recv_uid);
				out.WriteData(chat_private_ack->msg.c_str(),
					          chat_private_ack->msg.length());
				packet = (char*) out.GetData();
			}
			break;


		case PACKET_USER_LOGIN_ERROR:
		case PACKET_USER_CHAT_ERROR:
			{
				struct ChatERR* chat_err = (struct ChatERR*)packet_head;
				data_length = CHAT_ERR_SIZE;
				packet_length = PACKET_HEAD_LENGTH + data_length;
				packet = new char[packet_length];
				DataOutPacket out(false,packet_length);
				BULDHEAD();
				out.WriteData(chat_err->msg.c_str(),chat_err->msg.length());
				packet = (char*) out.GetData();
			}
			break;

	}
}

#define  BUILDPACKHEAD(pack)                     \
	pack->packet_length = inpacket.Read32();     \
    pack->operate_code = inpacket.Read32();      \
    pack->operate_result = inpacket.Read32();    \
    pack->data_length = inpacket.Read32();       \
    pack->reserved = inpacket.Read32();          \
	if(packet_length == PACKET_HEAD_LENGTH       \
       &&data_length==0)                         \
	   break;

bool ChatPack::UnPacketStream(const void *packet_stream, 
							  int packet_stream_length, 
                              struct PacketHead **packhead){
								  
	bool r = true;
	if (packet_stream_length < PACKET_HEAD_LENGTH)
		return false;

	int32 packet_length = ((struct PacketHead *) packet_stream)->packet_length;
	int32 operate_code = ((struct PacketHead *) packet_stream)->operate_code;
	int32 operate_result = ((struct PacketHead *) packet_stream)->operate_result;
	int32 data_length = ((struct PacketHead *) packet_stream)->data_length;
	int32 reserved = ((struct PacketHead *) packet_stream)->reserved;
	if (packet_length < PACKET_HEAD_LENGTH 
		|| packet_length != packet_stream_length
		|| data_length != packet_length - PACKET_HEAD_LENGTH)
		return false;

	char *data = (char *)packet_stream + PACKET_HEAD_LENGTH;
	switch (operate_code){
		case PACKET_LOAD_BALANCING_REQ:
			{
				struct LoadBalancingREQ* lb_req = new struct LoadBalancingREQ;
				*packhead = (struct PacketHead*)lb_req;
				DataInPacket inpacket((char*)packet_stream,packet_stream_length);
				BUILDPACKHEAD(lb_req);
				int32 temp;
				memcpy(lb_req->token,inpacket.ReadData(temp),TOKEN_LEN - 1);
				lb_req->token[TOKEN_LEN - 1] = '\0';
			}
			break;

		case PACKET_LOAD_BALANCING_ACK:
			{
				struct LoadBalancingACK* lb_ack = new struct LoadBalancingACK;
				*packhead = (struct PacketHead*) lb_ack;
				DataInPacket inpacket((char*)packet_stream,packet_stream_length);
				BUILDPACKHEAD(lb_ack);
				int32 temp;
				memcpy(lb_ack->ip,inpacket.ReadData(temp),IP_LEN - 1);
				lb_ack->ip[IP_LEN - 1] = '\0';
			}
			break;

		case PACKET_USER_LOGIN_REQ:
			{
				struct UserLoginREQ* user_login_req = new struct UserLoginREQ;
				*packhead = (struct PacketHead*) user_login_req;
				DataInPacket inpacket((char*)packet_stream,packet_stream_length);
				BUILDPACKHEAD(user_login_req);
				user_login_req->type = inpacket.Read8();
				user_login_req->net_type = inpacket.Read8();
				user_login_req->source = inpacket.Read8();
				user_login_req->hide = inpacket.Read8();
				user_login_req->user_id = inpacket.Read64();
				int32 temp;
				memcpy(user_login_req->token,inpacket.ReadData(temp),TOKEN_LEN -1);
				user_login_req->token[TOKEN_LEN - 1] = '\0';
			}
			break;

		case PACKET_USER_LOGIN_ACK:
			{
				struct UserLoginACK* user_login_ack = new struct UserLoginACK;
				*packhead = (struct PacketHead*)user_login_ack;
				DataInPacket inpacket((char*)packet_stream,packet_stream_length);
				BUILDPACKHEAD(user_login_ack);
				user_login_ack->session = inpacket.Read64();
				int32 temp;
				memcpy(user_login_ack->token,inpacket.ReadData(temp),TOKEN_LEN - 1);
				user_login_ack->token [TOKEN_LEN - 1] = '\0';
			}
			break;

		case PACKET_USER_PRIVATE_CHAT_REQ:
			{
				struct ChatPrivateREQ* chat_private_req = new struct ChatPrivateREQ;
				*packhead = (struct PacketHead*)chat_private_req;
				DataInPacket inpacket((char*)packet_stream,packet_stream_length);
				BUILDPACKHEAD(chat_private_req);
				int32 stringlen = data_length - (3 * 8 + TOKEN_LEN - 1);
				char* str = (char*)calloc(1,stringlen + 1);
				chat_private_req->send_uid = inpacket.Read64();
				chat_private_req->recv_uid = inpacket.Read64();
				chat_private_req->session = inpacket.Read64();
				int32 temp;
				memcpy(chat_private_req->token,inpacket.ReadData(temp),TOKEN_LEN -1);
				chat_private_req->token[TOKEN_LEN - 1] = '\0';
				memcpy(str,inpacket.ReadData(temp),stringlen);
				str[stringlen] = '\0';
				chat_private_req->msg.assign(str);
				if (str){free(str);}
			}
			break;

		case PACKET_USER_PRIVATE_CHAT_ACK:
			{
				struct ChatPrivateACK* chat_private_ack = new struct ChatPrivateACK;
				*packhead = (struct PacketHead*)chat_private_ack;
				DataInPacket inpacket((char*)packet_stream,packet_stream_length);
				BUILDPACKHEAD(chat_private_ack);
				int32 stringlen = data_length - (2 * 8);
				char* str = (char*)calloc(1,stringlen + 1);
				chat_private_ack->send_uid = inpacket.Read64();
				chat_private_ack->recv_uid = inpacket.Read64();
				int32 temp;
				memcpy(str,inpacket.ReadData(temp),stringlen);
				str[stringlen] = '\0';
				chat_private_ack->msg.assign(str);
				if (str){free(str);}
			}
			break;

		case PACKET_USER_LOGIN_ERROR:
		case PACKET_USER_CHAT_ERROR:
			{
				struct ChatERR* chat_err = new struct ChatERR;
				*packhead = (struct PacketHead*)chat_err;
				DataInPacket inpacket((char*)packet_stream,packet_stream_length);
				BUILDPACKHEAD(chat_err);
				int32 stringlen = data_length;
				char* str = (char*)calloc(1,stringlen + 1);
				int32 temp;
				memcpy(str,inpacket.ReadData(temp),stringlen);
				str[stringlen] = '\0';
				chat_err->msg.assign(str);
				if (str){free(str);}
			}
			break;

	}
}

#define DUMPPACKBUF     4096

#define PRINT_TITLE(v)   \
	j += snprintf (buf + j, DUMPPACKBUF - j, "\n-------- %s --------\n", v)

#define PRINT_END(v)     \
	j += snprintf (buf + j, DUMPPACKBUF - j, "-------- %s --------\n", v)

#define PRINT_INT(v)    \
	j += snprintf (buf + j, DUMPPACKBUF - j, "\t%s = %d\n", #v, (int)v)

#define PRINT_INT64(v)    \
	j += snprintf (buf + j, DUMPPACKBUF - j, "\t%s = %lld\n", #v, (int64_t)v)

#define PRINT_STRING(v) \
	j += snprintf (buf + j, DUMPPACKBUF - j, "\t%s = %s\n", #v, (const char *)v)

#define DUMPHEAD()	            \
	PRINT_INT (packet_length);	\
	PRINT_INT (operate_code);	\
	PRINT_INT (operate_result);	\
	PRINT_INT (data_length);    \
	PRINT_INT (reserved);       \
	if (packet_length == PACKET_HEAD_LENGTH \
	&& data_length == 0)    \
	break;


bool ChatPack::DumpPacket(const struct PacketHead *packhead){
#if defined DEBUG || defined _DEBUG
	int32 packet_length = packhead->packet_length;
	int32 operate_code = packhead->operate_code;
	int32 operate_result = packhead->operate_result;
	int32 data_length = packhead->data_length;
	int32 reserved = packhead->reserved;

	char buf[DUMPPACKBUF];
	buf[0] = '\0';
	int32 j = 0;

	switch (operate_code){
		case PACKET_LOAD_BALANCING_REQ:
			{
				struct LoadBalancingREQ* lb_req = (struct LoadBalancingREQ*)packhead;
				PRINT_TITLE("struct LoadBalancingREQ Dump Begin");
				DUMPHEAD();
				PRINT_STRING(lb_req->token);
				PRINT_END("struct LoadBalancingREQ Dump End");
			}
			break;

		case PACKET_LOAD_BALANCING_ACK:
			{
				struct LoadBalancingACK* lb_ack = (struct LoadBalancingACK*)packhead;
				PRINT_TITLE("struct LoadBalancingACK Dump Begin");
				DUMPHEAD();
				PRINT_STRING(lb_ack->ip);
				PRINT_END("struct LoadBalancingREQ Dump End");
			}
			break;

		case PACKET_USER_LOGIN_REQ:
			{
				struct UserLoginREQ* user_login_req = (struct UserLoginREQ*)packhead;
				PRINT_TITLE("struct UserLoginREQ Dump Begin");
				DUMPHEAD();
				PRINT_INT(user_login_req->type);
				PRINT_INT(user_login_req->net_type);
				PRINT_INT(user_login_req->source);
				PRINT_INT(user_login_req->hide);
				PRINT_INT(user_login_req->user_id);
				PRINT_STRING(user_login_req->token);
				PRINT_END("struct UserLoginREQ Dump End");
			}
			break;

		case PACKET_USER_LOGIN_ACK:
			{
				struct UserLoginACK* user_login_ack = (struct UserLoginACK*)packhead;
				PRINT_TITLE("struct UserLoginREQ Dump Begin");
				DUMPHEAD();
				PRINT_INT(user_login_ack->session);
				PRINT_STRING(user_login_ack->token);
				PRINT_END("struct UserLoginACK Dump End");
			}
			break;

		case PACKET_USER_PRIVATE_CHAT_REQ:
			{
				struct ChatPrivateREQ* chat_private_req = (struct ChatPrivateREQ*)packhead;
				PRINT_TITLE("struct ChatPrivateREQ Dump Begin");
				DUMPHEAD();
				PRINT_INT(chat_private_req->send_uid);
				PRINT_INT(chat_private_req->recv_uid);
				PRINT_INT(chat_private_req->session);
				PRINT_STRING(chat_private_req->token);
				PRINT_STRING(chat_private_req->msg.c_str());
				PRINT_END("struct ChatPrivateREQ Dump End");
			}
			break;

		case PACKET_USER_PRIVATE_CHAT_ACK:
			{
				struct ChatPrivateACK* chat_private_ack = (struct ChatPrivateACK*)packhead;
				PRINT_TITLE("struct ChatPrivateACK Dump Begin");
				DUMPHEAD();
				PRINT_INT(chat_private_ack->send_uid);
				PRINT_INT(chat_private_ack->recv_uid);
				PRINT_STRING(chat_private_ack->msg.c_str());
				PRINT_END("struct ChatPrivateACK Dump End");
			}
			break;

		case PACKET_USER_LOGIN_ERROR:
		case PACKET_USER_CHAT_ERROR:
			{
				struct ChatERR* chat_err = (struct ChatERR*)packhead;
				PRINT_TITLE("struct ChatERR Dump Begin");
				DUMPHEAD();
				PRINT_STRING(chat_err->msg.c_str());
				PRINT_END("struct ChatERR Dump End");
			}
			break;
	}
#endif

}

}
