#include "protocol.h"
#include "comm_head.h"
#include "protocol/data_packet.h"
#include "base/logic_comm.h"

bool ProtocolPack::UnpackStream(const void *packet_stream, int len, 
                                        struct PacketHead**packhead ){

	bool r = true;
	if (len < PACKET_HEAD_LENGTH)
		return false;

	int32 packet_length = ((struct PacketHead *) packet_stream)->packet_length;
	int32 packet_operate = ((struct PacketHead *) packet_stream)->operate_code;
	int32 data_length = ((struct PacketHead *) packet_stream)->data_length;
	int32 is_zip = ((struct PacketHead *) packet_stream)->is_zip;
	char* data = NULL;
	if (packet_length < PACKET_HEAD_LENGTH
		||packet_length!= len
		||data_length != packet_length - PACKET_HEAD_LENGTH){
			LOG_ERROR2("packet_length [%d] data_length[%d] PACK_HEAD_LENGTH [%d]",
				packet_length,data_length,PACKET_HEAD_LENGTH);
			return false;
	}

	//ÊÇ·ñÑ¹Ëõ
	if (is_zip)
		data = (char*)packet_stream + PACKET_HEAD_LENGTH;
	else
		data = (char*)packet_stream + PACKET_HEAD_LENGTH;

	switch(packet_operate){
		case USER_LOGIN:
			{
				struct UserLogin* vUserLogin = new struct UserLogin;
				*packhead = (struct PacketHead*)vUserLogin;

				BUILDPACKHEAD();

				vUserLogin->platform_id = in.Read64();
				vUserLogin->user_id = in.Read64();
				vUserLogin->net_type = in.Read8();
				vUserLogin->user_type = in.Read8();
				vUserLogin->device = in.Read8();
				int temp = 0;
				memcpy(vUserLogin->token,in.ReadData(TOKEN_LEN,temp),
					TOKEN_LEN);
				vUserLogin->token[TOKEN_LEN - 1] = '\0';
			}
			break;
		case USER_LOGIN_SUCESS:
			{
				struct UserLoginSucess* vUserLoginSucess = new struct UserLoginSucess;
				*packhead = (struct PacketHead*)vUserLoginSucess;

				BUILDPACKHEAD();
				vUserLoginSucess->platform_id = in.Read64();
				vUserLoginSucess->user_id = in.Read64();
				vUserLoginSucess->nick_number = in.Read64();
				int temp = 0;
				memcpy(vUserLoginSucess->token,in.ReadData(TOKEN_LEN,temp),
					TOKEN_LEN);
				vUserLoginSucess->token[TOKEN_LEN - 1] = '\0';

				memcpy(vUserLoginSucess->nickname,in.ReadData(NICKNAME_LEN,temp),
					NICKNAME_LEN);
				vUserLoginSucess->nickname[NICKNAME_LEN - 1] = '\0';

				memcpy(vUserLoginSucess->head_url,in.ReadData(HEAD_URL_LEN,temp),
					NICKNAME_LEN);
				vUserLoginSucess->head_url[HEAD_URL_LEN - 1] = '\0';
			}
			break;

		case USER_LOGIN_FAILED:
			{
				struct ChatFailed* vChatFailed = new struct ChatFailed;
				*packhead = (struct PacketHead*)vChatFailed;
				BUILDPACKHEAD();
				vChatFailed->platform_id = in.Read64();
				int temp = 0;
				int strlen = vChatFailed->data_length;
				char* str = new char[strlen+1];
				memcpy(str,in.ReadData(strlen,temp),strlen);
				vChatFailed->error_msg.assign(str,strlen);
				if(str){
					delete [] str;
					str = NULL;
				}
			}
			break;
		case USER_QUIT:
			{
				struct UserQuit* vUserQuit = new struct UserQuit;
				*packhead = (struct PacketHead*)vUserQuit;
				BUILDPACKHEAD();
				vUserQuit->platform_id = in.Read64();
				vUserQuit->user_id = in.Read64();
				vUserQuit->session =in.Read64();
				int temp = 0;
				memcpy(vUserQuit->token,in.ReadData(TOKEN_LEN,temp),TOKEN_LEN);
				vUserQuit->token[TOKEN_LEN - 1] = '\0';
			}
			break;
		case USER_NOTIFICATION_QUIT:
			{
				struct UserQuitNotification* vUserQuitNotification 
					= new struct UserQuitNotification;
				*packhead = (struct PacketHead*)vUserQuitNotification;
				BUILDPACKHEAD();
				vUserQuitNotification->platform_id = in.Read64();
				vUserQuitNotification->user_id = in.Read64();
			}
			break;
		case REQ_OPPOSITION_INFO:
			{
				struct ReqOppstionInfo* vReqOppstionInfo 
					= new struct ReqOppstionInfo;
				*packhead = (struct PacketHead*)vReqOppstionInfo;
				BUILDPACKHEAD ();
				vReqOppstionInfo->platform_id = in.Read64();
				vReqOppstionInfo->user_id = in.Read64();
				vReqOppstionInfo->oppostion_id = in.Read64();
				vReqOppstionInfo->type = in.Read16();
				int temp = 0;
				memcpy(vReqOppstionInfo->token,
					in.ReadData(TOKEN_LEN,temp),TOKEN_LEN);
				vReqOppstionInfo->token[TOKEN_LEN - 1] = '\0';
			}
			break;

		case TEXT_CHAT_PRIVATE_RECV:
			{
				struct TextChatPrivateRecv* vTextChatPrivateRecv
					= new struct TextChatPrivateRecv;
				*packhead = (struct PacketHead*)vTextChatPrivateRecv;
				BUILDPACKHEAD();
				vTextChatPrivateRecv->platform_id = in.Read64();
				vTextChatPrivateRecv->send_user_id = in.Read64();
				vTextChatPrivateRecv->recv_user_id = in.Read64();
				int len = vTextChatPrivateRecv->data_length - sizeof(int64) * 2;
				int temp = 0;
				char* str = new char[len];
				memcpy(str,in.ReadData(len,temp),len);
				vTextChatPrivateRecv->content.assign(str,len);
				if(str){
					delete [] str;
					str = NULL;
				}
			}
			break;
		case TEXT_CHAT_PRIVATE_SEND:
			{
				struct TextChatPrivateSend* vTextChatPrivateSend = 
					new struct TextChatPrivateSend;
				*packhead = (struct PacketHead*)vTextChatPrivateSend;
				BUILDPACKHEAD();
				vTextChatPrivateSend->platform_id = in.Read64();
				vTextChatPrivateSend->send_user_id = in.Read64();
				vTextChatPrivateSend->recv_user_id = in.Read64();
				vTextChatPrivateSend->session = in.Read64();
				int temp = 0;
				memcpy(vTextChatPrivateSend->token,in.ReadData(TOKEN_LEN,temp),
					TOKEN_LEN);
				vTextChatPrivateSend->token[TOKEN_LEN - 1] = '\0';
				int len = vTextChatPrivateSend->data_length - sizeof(int64) * 3 - TOKEN_LEN;
				char* str = new char[len];
				memcpy(str,in.ReadData(len,temp),len);
				vTextChatPrivateSend->content.assign(str,len);
				if(str){
					delete [] str;
					str = NULL;
				}
			}
			break;

		case PACKET_CONFIRM:
			{
				struct PacketConfirm* vPacketConfirm = 
					new struct PacketConfirm;
				*packhead = (struct PacketHead*)vPacketConfirm;
				BUILDPACKHEAD();

				vPacketConfirm->platform_id = in.Read64();
				vPacketConfirm->send_user_id = in.Read64();
				vPacketConfirm->recv_user_id = in.Read64();
				vPacketConfirm->session_id = in.Read64();
				int temp = 0;
				memcpy(vPacketConfirm->token,
					in.ReadData(TOKEN_LEN,temp),TOKEN_LEN);
				vPacketConfirm->token[TOKEN_LEN - 1] = '\0';

			}
			break;

		case GET_OPPOSITION_INFO:
			{
			    struct OppositionInfo* vOppositionInfo = 
					new struct OppositionInfo;
				*packhead = (struct PacketHead*)vOppositionInfo;
				BUILDPACKHEAD();
				//
				vOppositionInfo->platform_id = in.Read64();
				vOppositionInfo->oppo_id = in.Read64();
				vOppositionInfo->oppo_nick_number = in.Read64();
				vOppositionInfo->session = in.Read64();
				vOppositionInfo->oppo_type = in.Read16();
				int temp = 0;
				memcpy(vOppositionInfo->oppo_nickname,
					in.ReadData(NICKNAME_LEN,temp),NICKNAME_LEN);
				vOppositionInfo->oppo_nickname[NICKNAME_LEN - 1] = '\0';

				memcpy(vOppositionInfo->oppo_user_head,
					in.ReadData(HEAD_URL_LEN,temp),HEAD_URL_LEN);
				vOppositionInfo->oppo_user_head[HEAD_URL_LEN - 1] = '\0';
				int list_size = data_length - sizeof(int64) * 3 - NICKNAME_LEN -TOKEN_LEN;
				int i = 0;
				int nums = list_size/ OPPSITIONINFO_SIZE;
				int len = 0;

				for (;i<nums;i++){
					struct Oppinfo* opp_info = new struct Oppinfo;
					int temp = 0;
					opp_info->user_id - in.Read64();
					opp_info->user_nicknumber = in.Read64();
					memcpy(opp_info->nickname,in.ReadData(NICKNAME_LEN,temp),
						NICKNAME_LEN);
					opp_info->nickname[NICKNAME_LEN -1] = '\0';

					memcpy(opp_info->user_head,in.ReadData(HEAD_URL_LEN,temp),
						HEAD_URL_LEN);
					opp_info->user_head[HEAD_URL_LEN] = '\0';
					vOppositionInfo->opponfo_list.push_back(opp_info);
				}

			}
			break;
		default:
			r = false;
			break;
	}
	return r;
}

bool ProtocolPack::PackStream(const struct PacketHead* packhead,void** packet_stream,
							  int32* packet_stream_length){
	bool r = true;

	int32 packet_length = packhead->packet_length;
	int32 operate_code = packhead->operate_code;
	int32 data_length = packhead->data_length;
	int32 current_time = packhead->current_time;
	int8  is_zip = packhead->is_zip;
	int16 msg_type = packhead->msg_type;
	int32 reserverd = packhead->reserverd;

	char* packet = NULL;
	char* data = NULL;

	switch(operate_code){
		case USER_LOGIN:
			{
				struct UserLogin* vUserLogin = (struct UserLogin*)packhead;
				BUILDHEAD(USER_LOGIN_SIZE);

				out.Write64(vUserLogin->platform_id);
				out.Write64(vUserLogin->user_id);
				out.Write8(vUserLogin->net_type);
				out.Write8(vUserLogin->user_type);
				out.Write8(vUserLogin->device);
				out.WriteData(vUserLogin->token,TOKEN_LEN);

				packet = (char*)out.GetData();
			}
			break;
		case USER_LOGIN_SUCESS:
			{
				struct UserLoginSucess* vUserLoginSucess = (struct UserLoginSucess*)packhead;
				BUILDHEAD(USER_LOGIN_SUCESS_SIZE);
				out.Write64(vUserLoginSucess->platform_id);
				out.Write64(vUserLoginSucess->user_id);
				out.Write64(vUserLoginSucess->nick_number);
				out.WriteData(vUserLoginSucess->token,TOKEN_LEN);
				out.WriteData(vUserLoginSucess->nickname,NICKNAME_LEN);
				out.WriteData(vUserLoginSucess->head_url,HEAD_URL_LEN);

				packet = (char*)out.GetData();
			}
			break;
		case USER_LOGIN_FAILED:
			{
				struct ChatFailed* vChatFailed = (struct ChatFailed*)packhead;
				BUILDHEAD(USER_LOGIN_FAILED_SIZE);
				out.Write64(vChatFailed->platform_id);
				out.WriteData(vChatFailed->error_msg.c_str(),
							  vChatFailed->error_msg.length());

				packet = (char*)out.GetData();
			}
			break;
		case USER_QUIT:
			{
				struct UserQuit* vUserQuit = (struct UserQuit*)packhead;
				BUILDHEAD(USER_QUIT_SIZE);
				out.Write64(vUserQuit->platform_id);
				out.Write64(vUserQuit->user_id);
				out.Write64(vUserQuit->session);
				out.WriteData(vUserQuit->token,TOKEN_LEN);
				packet = (char*)out.GetData();

			}
			break;
		case USER_NOTIFICATION_QUIT:
			{
				struct UserQuitNotification* vUserQuitNotification 
					= (struct UserQuitNotification*)packhead;
				BUILDHEAD(USER_NOTIFICATION_QUIT_SIZE);
				out.Write64(vUserQuitNotification->platform_id);
				out.Write64(vUserQuitNotification->user_id);
				packet = (char*)out.GetData();
			}
			break;
		case REQ_OPPOSITION_INFO:
			{
				struct ReqOppstionInfo* vReqOppstionInfo
					= (struct ReqOppstionInfo*)packhead;
				BUILDHEAD(REQ_OPPOSITION_INFO_SIZE);
				out.Write64(vReqOppstionInfo->platform_id);
				out.Write64(vReqOppstionInfo->user_id);
				out.Write64(vReqOppstionInfo->oppostion_id);
				out.Write16(vReqOppstionInfo->type);
				out.WriteData(vReqOppstionInfo->token,TOKEN_LEN);
				packet = (char*)out.GetData();

			}
			break;
		case TEXT_CHAT_PRIVATE_SEND:
			{
				struct TextChatPrivateSend* vTextChatPrivateSend 
					= (struct TextChatPrivateSend*)packhead;
				BUILDHEAD(TEXTCHATPRIVATESEND_SIZE);
				out.Write64(vTextChatPrivateSend->platform_id);
				out.Write64(vTextChatPrivateSend->send_user_id);
				out.Write64(vTextChatPrivateSend->recv_user_id);
				out.Write64(vTextChatPrivateSend->session);
				out.WriteData(vTextChatPrivateSend->token,TOKEN_LEN);
				out.WriteData(vTextChatPrivateSend->content.c_str(),vTextChatPrivateSend->content.length());
				packet = (char*)out.GetData();

			}
			break;
		case TEXT_CHAT_PRIVATE_RECV:
			{
				struct TextChatPrivateRecv* vTextChatPrivateRecv 
					= (struct TextChatPrivateRecv*)packhead;
				BUILDHEAD(TEXTCHATPRIVATERECV_SIZE);
				out.Write64(vTextChatPrivateRecv->platform_id);
				out.Write64(vTextChatPrivateRecv->send_user_id);
				out.Write64(vTextChatPrivateRecv->recv_user_id);
				out.WriteData(vTextChatPrivateRecv->content.c_str(),
					vTextChatPrivateRecv->content.length());
				packet = (char*)out.GetData();
			}
			break;
		case PACKET_CONFIRM:
			{
				struct PacketConfirm* vPacketConfirm 
					= (struct PacketConfirm*)packhead;
				BUILDHEAD(PACKET_CONFIRM_SIZE);
				out.Write64(vPacketConfirm->platform_id);
				out.Write64(vPacketConfirm->send_user_id);
				out.Write64(vPacketConfirm->recv_user_id);
				out.Write64(vPacketConfirm->session_id);
				out.WriteData(vPacketConfirm->token,TOKEN_LEN);
				packet = (char*)out.GetData();
			}
			break;
		case GET_OPPOSITION_INFO:
			{
				struct OppositionInfo* vOppostionInfo =
					(struct OppositionInfo*)packhead;
				int len = 0;
				data_length = vOppostionInfo->opponfo_list.size()* OPPSITIONINFO_SIZE + sizeof(int64) * 2 + sizeof(int16) + NICKNAME_LEN + HEAD_URL_LEN;
				BUILDHEAD(data_length);
				out.Write64(vOppostionInfo->platform_id);
				out.Write64(vOppostionInfo->oppo_id);
				out.Write64(vOppostionInfo->oppo_nick_number);
				out.Write64(vOppostionInfo->session);
				out.Write16(vOppostionInfo->oppo_type);
				out.WriteData(vOppostionInfo->oppo_nickname,NICKNAME_LEN);
				out.WriteData(vOppostionInfo->oppo_user_head,HEAD_URL_LEN);
				std::list<struct Oppinfo*>::iterator it = vOppostionInfo->opponfo_list.begin();
				for (;it!=vOppostionInfo->opponfo_list.end();it++){
					out.Write64((*it)->user_id);
					out.Write64((*it)->user_nicknumber);
					out.WriteData((*it)->nickname,NICKNAME_LEN);
					out.WriteData((*it)->user_head,HEAD_URL_LEN);
				}

				packet = (char*)out.GetData();

			}
			break;
		default:
			r = false;
			break;
	}

	*packet_stream = (void *)packet;

	if (packhead->packet_length == PACKET_HEAD_LENGTH
		&& packhead->data_length == 0)
		*packet_stream_length = PACKET_HEAD_LENGTH;
	else
		*packet_stream_length = packet_length;
	return r;
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
	PRINT_INT (data_length);    \
	PRINT_INT (reserved);       \
	if (packet_length == PACKET_HEAD_LENGTH \
	&& data_length == 0)    \
	break;


void ProtocolPack::DumpPacket(const struct PacketHead *packhead){
#if !defined DEBUG || defined _DEBUG
	int32 packet_length = packhead->packet_length;
	int32 operate_code = packhead->operate_code;
	int32 data_length = packhead->data_length;
	int32 reserved = packhead->reserverd;
	char buf[DUMPPACKBUF];
	bool r = false;
	buf[0] = '\0';
	int j = 0;

	switch(operate_code){
		case USER_LOGIN:
			{
				struct UserLogin* vUserLogin = (struct UserLogin*)packhead;
				PRINT_TITLE("struct UserLogin Dump Begin");
				DUMPHEAD ();
				PRINT_INT64(vUserLogin->platform_id);
				PRINT_INT64(vUserLogin->user_id);
				PRINT_INT(vUserLogin->device);
				PRINT_INT(vUserLogin->net_type);
				PRINT_INT(vUserLogin->user_type);
				PRINT_STRING(vUserLogin->token);
				PRINT_END ("struct ErrorReponse Dump End");
			}
			break;

		case USER_LOGIN_SUCESS:
			{
				struct UserLoginSucess* vUserLoginSucess = (struct UserLoginSucess*)packhead;
				PRINT_TITLE("Strut vUserLoginSucess Dump Begin");
				DUMPHEAD ();
				PRINT_INT64(vUserLoginSucess->platform_id);
				PRINT_INT64(vUserLoginSucess->user_id);
				PRINT_INT64(vUserLoginSucess->nick_number);
				PRINT_STRING(vUserLoginSucess->token);
				PRINT_STRING(vUserLoginSucess->nickname);
				PRINT_STRING(vUserLoginSucess->head_url);
				PRINT_END ("Strut vUserLoginSucess Dump End");
			}
			break;

		case USER_LOGIN_FAILED:
			{
				struct ChatFailed* vChatFailed = (struct ChatFailed*)packhead;
				PRINT_TITLE("struct ChatFailed Dump Begin");
				DUMPHEAD ();
				PRINT_INT64(vChatFailed->platform_id);
				PRINT_STRING(vChatFailed->error_msg.c_str());
				PRINT_END ("struct ChatFailed Dump End");
			}
			break;

		case USER_QUIT:
			{
				struct UserQuit* vUserQuit = (struct UserQuit*)packhead;
				PRINT_TITLE("struct UserQuit Dump Begin");
				DUMPHEAD ();
				PRINT_INT64(vUserQuit->platform_id);
				PRINT_INT64(vUserQuit->user_id);
				PRINT_INT64(vUserQuit->session);
				PRINT_STRING(vUserQuit->token);
				PRINT_END ("struct UserQuit Dump End");
			}
			break;
		case USER_NOTIFICATION_QUIT:
			{
				struct UserQuitNotification* vUserQuitNotification =
					(struct UserQuitNotification*)packhead;
				PRINT_TITLE("struct UserQuitNotification Dump Begin");
				DUMPHEAD ();

				PRINT_INT64(vUserQuitNotification->platform_id);
				PRINT_INT64(vUserQuitNotification->user_id);
				PRINT_END("struct UserQuitNotification Dump End");
			}
			break;
		case REQ_OPPOSITION_INFO:
			{
				struct ReqOppstionInfo* vReqOppstionInfo =
					(struct ReqOppstionInfo*)packhead;
				PRINT_TITLE("struct UserQuitNotification Dump Begin");
				PRINT_INT64(vReqOppstionInfo->platform_id);
				PRINT_INT64(vReqOppstionInfo->user_id);
				PRINT_INT64(vReqOppstionInfo->oppostion_id);
				PRINT_INT(vReqOppstionInfo->type);
				PRINT_STRING(vReqOppstionInfo->token);
				PRINT_END("struct UserQuitNotification Dump End");
			}
			break;

		case GET_OPPOSITION_INFO:
			{
				struct OppositionInfo* vOppostionInfo = 
					(struct OppositionInfo*)packhead;
				std::list<struct Oppinfo*>::iterator it =
					vOppostionInfo->opponfo_list.begin();
				DUMPHEAD();
				PRINT_TITLE("struct OppositionInfo DumpBegin");
				PRINT_INT64(vOppostionInfo->platform_id);
				PRINT_INT64(vOppostionInfo->oppo_id);
				PRINT_INT64(vOppostionInfo->oppo_nick_number);
				PRINT_INT64(vOppostionInfo->session);
				PRINT_INT(vOppostionInfo->oppo_type);
				PRINT_STRING(vOppostionInfo->oppo_nickname);
				PRINT_STRING(vOppostionInfo->oppo_user_head);
				for(;it!=vOppostionInfo->opponfo_list.end();it++){
					PRINT_INT64((*it)->user_id);
					PRINT_INT64((*it)->user_nicknumber);
					PRINT_INT64((*it)->nickname);
					PRINT_INT64((*it)->user_head);
				}

				PRINT_END ("struct OppositionInfo Dump End");
			}
			break;
		case TEXT_CHAT_PRIVATE_SEND:
			{
				struct TextChatPrivateSend* vTextChatPrivateSend = 
					(struct TextChatPrivateSend*)packhead;
				PRINT_TITLE("struct TextChatPrivateSend Dump Begin");
				PRINT_INT64(vTextChatPrivateSend->platform_id);
				PRINT_INT64(vTextChatPrivateSend->send_user_id);
				PRINT_INT64(vTextChatPrivateSend->recv_user_id);
				PRINT_INT64(vTextChatPrivateSend->session);
				PRINT_STRING(vTextChatPrivateSend->token);
				PRINT_STRING(vTextChatPrivateSend->content.c_str());
				PRINT_END("struct TextChatPrivateSend Dump End");
			}
			break;

		case TEXT_CHAT_PRIVATE_RECV:
			{
				struct TextChatPrivateRecv* vTextChatPrivateRecv = 
					(struct TextChatPrivateRecv*)packhead;
				PRINT_TITLE("struct TextChatPrivateRecv Dump Begin");
				PRINT_INT64(vTextChatPrivateRecv->platform_id);
				PRINT_INT64(vTextChatPrivateRecv->send_user_id);
				PRINT_INT64(vTextChatPrivateRecv->recv_user_id);
				PRINT_STRING(vTextChatPrivateRecv->content.c_str());
				PRINT_END("struct TextChatPrivateRecv Dump End");
			}
			break;
		case PACKET_CONFIRM:
			{
				struct PacketConfirm* vPacketConfirm =
					(struct PacketConfirm*)packhead;
				PRINT_TITLE("struct PacketConfirm Dump Begin");
				PRINT_INT64(vPacketConfirm->platform_id);
				PRINT_INT64(vPacketConfirm->send_user_id);
				PRINT_INT64(vPacketConfirm->recv_user_id);
				PRINT_INT64(vPacketConfirm->session_id);
				PRINT_STRING(vPacketConfirm->token);
				PRINT_END("struct PacketConfirm End");
			}
			break;
		default:
			r = false;
			break;
	}
#endif
}



void ProtocolPack::HexEncode(const void *bytes, size_t size){
	struct PacketHead* head = (struct PacketHead*)bytes;
	static const char kHexChars[] = "0123456789ABCDEF";
	std::string sret(size*3,'\0');
	for (size_t i = 0;i<size;++i){
		char b = reinterpret_cast<const char*>(bytes)[i];
		sret[(i*3)] = kHexChars[(b>>4) & 0xf];
		sret[(i*3)+1]=kHexChars[b&0xf];
		if ((((i*3)+2+1)%12)!=0)
			sret[(i * 3) + 2] = '\40';
		else
			sret[(i * 3) + 2] = '\n';
	}
	struct PackHead *packet = NULL;
}

