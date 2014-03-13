#include "im_mgr.h"
#include "logic_unit.h"
#include "dic_comm.h"
#include "db_comm.h"
#include "chat_cache_manager.h"
#include "base/error_code.h"
#include "json/json.h"
#include "base/logic_comm.h"
#include <sstream>

namespace chat_logic{

IMSMgr::IMSMgr(){

}

IMSMgr::~IMSMgr(){

}

bool IMSMgr::OnCreatePrviateChat(struct server *srv,int socket,struct PacketHead *packet,
			const void *msg/*= NULL*/,int len /* = 0*/){
     return true;
}

bool IMSMgr::OnMessage(struct server *srv, int socket, struct PacketHead *packet,
        const void *msg/* = NULL*/, int len/* = 0*/){

	struct TextChatPrivateSend* private_send = (struct TextChatPrivateSend*)packet;
	chat_base::UserInfo send_user_info;
	chat_base::UserInfo recv_user_info;
	int64 msg_id = private_send->msg_id;
	time_t current_time;
	bool r = false;

	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
	r = chat_logic::LogicUnit::CheckToken(private_send->platform_id,private_send->send_user_id,
											(const char*)private_send->token);

	if (!r){//password error
		//senderror(socket,USER_LOGIN_FAILED,0,private_send->reserverd,MIG_CHAT_USER_PASSWORD_ERROR);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,private_send->reserverd,MIG_CHAT_USER_PASSWORD_ERROR,__FILE__,__LINE__);

		return false;
	}

	r = pc->GetUserInfos(private_send->platform_id,private_send->send_user_id,send_user_info);

	if(!r){
		//senderror(socket,USER_LOGIN_FAILED,0,private_send->send_user_id,MIG_CHAT_USER_NO_EXIST);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,private_send->send_user_id,MIG_CHAT_USER_NO_EXIST,__FILE__,__LINE__);
		return false;
	}

	r = pc->GetUserInfos(private_send->platform_id,private_send->recv_user_id,recv_user_info);

	if(!r){
		msg_id = base::SysRadom::GetInstance()->GetRandomID();
		current_time = time(NULL);
		PushMessage(private_send->platform_id,send_user_info,recv_user_info,private_send->content);
	}else {
		struct TextChatPrivateRecv private_recv;
		MAKE_HEAD(private_recv, TEXT_CHAT_PRIVATE_RECV,CHAT_TYPE,0,private_send->reserverd);
		private_recv.platform_id = private_send->platform_id;
		private_recv.send_user_id = private_send->send_user_id;
		private_recv.recv_user_id = private_send->recv_user_id;
		private_recv.content = private_send->content;
		msg_id = private_recv.msg_id;
		current_time = private_recv.current_time;
		//sendmessage(recv_user_info.socket(),&private_recv);
		logic::SomeUtils::SendMessage(recv_user_info.socket(),&private_recv,__FILE__,__LINE__);
	}

	// Leave Meassage

	return LeaveMessage(private_send->platform_id,msg_id,current_time,send_user_info,
			recv_user_info,private_send->content);
}


bool IMSMgr::LeaveMessage(const int64 platform_id,const int64 msg_id,
		const time_t current_time,
		const chat_base::UserInfo& send_userinfo,
		const chat_base::UserInfo& recv_userinfo,
		const std::string& message){

	//storage mysql
	std::stringstream os;
	os<<current_time;

	return chat_storage::DBComm::RecordMessage(platform_id,send_userinfo.user_id(),recv_userinfo.user_id(),
												msg_id,message,os.str());
}

// push to apple message center
bool IMSMgr::PushMessage(const int64 platform_id,const chat_base::UserInfo& send_userinfo,
		const chat_base::UserInfo& recv_userinfo,const std::string& message){

	std::stringstream os;
	std::string device_token;
	bool is_receive;
	unsigned begin_time;
	unsigned end_time;
	os<<recv_userinfo.nickname()<<":"<<message;
	//get decivce token and  begin_time/end_time
	chat_storage::RedisComm::GetUserPushConfig(recv_userinfo.user_id(),
			device_token,is_receive,begin_time,end_time);

	return chat_logic::HttpComm::PushMessage(device_token,message);
}

}

