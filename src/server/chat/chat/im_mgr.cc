#include "im_mgr.h"
#include "dic_comm.h"
#include "db_comm.h"
#include "chat_cache_manager.h"
#include "logic_unit.h"
#include "base/logic_comm.h"
#include "base/error_code.h"
#include "json/json.h"
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

bool IMSMgr::OnConfirmMessage(struct server *srv,int socket,struct PacketHead *packet,
					const void *msg/* = NULL*/, int len/* = 0*/){
	struct PacketConfirm* vPacketConfirm = (struct PacketConfirm*)packet;
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
	chat_base::UserInfo recv_user_info;
	pc->DelConfirmMessage(vPacketConfirm->platform_id,vPacketConfirm->msg_id);
	bool r = pc->GetUserInfos(vPacketConfirm->platform_id,vPacketConfirm->send_user_id,recv_user_info);
	if(!r)
		return false;
	struct PacketConfirm confirm;
	MAKE_HEAD(confirm, PACKET_CONFIRM,CHAT_TYPE,0,recv_user_info.session());
	confirm.platform_id = vPacketConfirm->platform_id;
	confirm.recv_user_id = vPacketConfirm->recv_user_id;
	confirm.send_user_id = vPacketConfirm->send_user_id;
	//confirm.token
	logic::SomeUtils::SendMessage(recv_user_info.socket(),&confirm,__FILE__,__LINE__);
    return true;
}

bool IMSMgr::OnMessage(struct server *srv, int socket, struct PacketHead *packet,
        const void *msg/* = NULL*/, int len/* = 0*/){

	struct TextChatPrivateSend* private_send = (struct TextChatPrivateSend*)packet;
	chat_base::UserInfo send_user_info;
	chat_base::UserInfo recv_user_info;
	chat_base::UserInfo userinfo;
	int64 msg_id = private_send->msg_id;
	bool is_push = false;
	time_t current_time;
	bool r = false;

	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();

	pc->GetUserInfos(private_send->platform_id,private_send->send_user_id,userinfo);

	if(!r)
		return false;

	r = chat_logic::LogicUnit::CheckChatToken(userinfo,private_send->token);

	if (!r){//password error
		//senderror(socket,USER_LOGIN_FAILED,0,vUserQuit->reserverd,MIG_CHAT_USER_PASSWORD_ERROR);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,private_send->reserverd,MIG_CHAT_USER_PASSWORD_ERROR,__FILE__,__LINE__);
		return false;
	}


	r = pc->GetUserInfos(private_send->platform_id,private_send->send_user_id,send_user_info);

	if(!r){
		senderror(socket,USER_LOGIN_FAILED,0,send_user_info.session(),MIG_CHAT_USER_NO_EXIST);
		return false;
	}

	r = pc->GetUserInfos(private_send->platform_id,private_send->recv_user_id,recv_user_info);
	if(!r){ // offline
		is_push = true;
		//get oppinfo_user_info from db
		if(!r)
			r = chat_logic::LogicUnit::GetUserInfo(private_send->platform_id,private_send->recv_user_id,recv_user_info);
		if (!r){//user vailed
			senderror(socket,USER_LOGIN_FAILED,0,send_user_info.session(),MIG_CHAT_USER_NO_EXIST);
			return false;
		}

	}

	if(is_push){
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
		pc->AddConfirmMessage(private_send->platform_id,private_recv.msg_id,send_user_info);

	}

	// Leave Meassage
	return LeaveMessage(private_send->platform_id,msg_id,current_time,send_user_info,
			recv_user_info,private_send->content);
}


bool IMSMgr::LeaveMessage(const int64 platform_id,const int64 msg_id,const time_t current_time,
		const chat_base::UserInfo& send_userinfo,const chat_base::UserInfo& recv_userinfo,
		const std::string& message){

	//storage mysql

	//std::stringstream os;
	//os<<current_time;
	std::string s_current_time;
	logic::SomeUtils::GetCurrntTimeFormat(s_current_time);

	//LOG_DEBUG2("current_time = %s msg_id = %lld send_id = %lld recv_id = %lld content = %s",
		//	s_current_time.c_str(),msg_id,send_userinfo.user_id(),recv_userinfo.user_id(),message.c_str());

	return chat_storage::DBComm::RecordMessage(platform_id,send_userinfo.user_id(),recv_userinfo.user_id(),
												msg_id,message,s_current_time);
}

// push to apple message center
bool IMSMgr::PushMessage(const int64 platform_id,const chat_base::UserInfo& send_userinfo,
		const chat_base::UserInfo& recv_userinfo,const std::string& message){

	std::stringstream os;
	std::string device_token;
	bool is_receive;
	unsigned begin_time;
	unsigned end_time;
	os<<send_userinfo.nickname()<<":"<<message;
	//get decivce token and  begin_time/end_time
	chat_storage::RedisComm::GetUserPushConfig(recv_userinfo.user_id(),
			device_token,is_receive,begin_time,end_time);

	return chat_logic::HttpComm::PushMessage(device_token,os.str());
}

}

