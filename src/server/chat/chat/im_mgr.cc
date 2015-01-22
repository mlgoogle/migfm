#include "im_mgr.h"
#include "dic_comm.h"
#include "db_comm.h"
#include "chat_cache_manager.h"
#include "logic_unit.h"
#include "queue/block_msg_queue.h"
#include "logic/cache_manager.h"
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
	confirm.msg_id = vPacketConfirm->private_msg_id;
	confirm.platform_id = vPacketConfirm->platform_id;
	confirm.recv_user_id = vPacketConfirm->recv_user_id;
	confirm.send_user_id = vPacketConfirm->send_user_id;
	//confirm.token
	logic::SomeUtils::SendMessage(recv_user_info.socket(),&confirm,__FILE__,__LINE__);
    return true;
}

bool IMSMgr::OnGroupMessage(struct server *srv, int socket, struct PacketHead *packet,
	           const void *msg, int len){
	struct MultiChatSend* vMultiChatSend = (struct MultiChatSend*)packet;
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
	chat_base::GroupInfo  groupinfo;
	int64 msg_id = 0;
	//获取群组信息
	bool r = pc->GetGroupInfos(vMultiChatSend->platform_id,vMultiChatSend->multi_id,groupinfo);
	if(!r)
		return false;
	//获取个人信息
	chat_base::UserInfo send_user_info;
	r = pc->GetUserInfos(vMultiChatSend->platform_id,vMultiChatSend->send_user_id,send_user_info);
	if(!r)
		return false;

	//群发
	chat_storage::RedisComm::GenaratePushMsgID(msg_id);
	struct MultiChatRecv multi_chat_recv;
	MAKE_HEAD(multi_chat_recv, MULTI_CHAT_RECV,CHAT_TYPE,0,vMultiChatSend->reserverd);
	multi_chat_recv.platform_id = vMultiChatSend->platform_id;
	multi_chat_recv.multi_id = vMultiChatSend->multi_id;
	multi_chat_recv.send_user_id = vMultiChatSend->send_user_id;
	//multi_chat_recv.send_nickname = send_user_info.nickname();
	logic::SomeUtils::SafeStrncpy(multi_chat_recv.send_nickname,NICKNAME_LEN,
			send_user_info.nickname().c_str(),send_user_info.nickname().length());
	multi_chat_recv.content = vMultiChatSend->content;
	pc->SendMeetingNotSelf(multi_chat_recv.platform_id,multi_chat_recv.multi_id,
			vMultiChatSend->send_user_id,vMultiChatSend->session,&multi_chat_recv);

	LeaveMessage(multi_chat_recv.platform_id,msg_id,DIMENSION_GROUP_CHAT,
			vMultiChatSend->multi_id,time(NULL),send_user_info,
			vMultiChatSend->content);
	return true;

}

bool IMSMgr::OnMessage(struct server *srv, int socket, struct PacketHead *packet,
        const void *msg/* = NULL*/, int len/* = 0*/){

	struct TextChatPrivateSend* private_send = (struct TextChatPrivateSend*)packet;
	chat_base::UserInfo send_user_info;
	chat_base::UserInfo recv_user_info;
	chat_base::UserInfo userinfo;
	int64 msg_id;
	bool is_push = false;
	time_t current_time;
	bool r = false;

	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
/*
	r = pc->GetUserInfos(private_send->platform_id,private_send->send_user_id,userinfo);

	if(!r)
		return false;
*/
	r = chat_logic::LogicUnit::CheckChatToken(userinfo,private_send->token);
	r = true;
	if (!r){//password error
		//senderror(socket,USER_LOGIN_FAILED,0,vUserQuit->reserverd,MIG_CHAT_USER_PASSWORD_ERROR);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,private_send->reserverd,MIG_CHAT_USER_PASSWORD_ERROR,__FILE__,__LINE__);
		return false;
	}

	if(private_send->send_user_id!=10000){
		r = pc->GetUserInfos(private_send->platform_id,private_send->send_user_id,send_user_info);

		if(!r){
			senderror(socket,USER_LOGIN_FAILED,0,send_user_info.session(),MIG_CHAT_USER_NO_EXIST);
			return false;
		}
	}else{
		r = chat_logic::LogicUnit::GetUserInfo(private_send->platform_id,private_send->send_user_id,send_user_info);
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
		chat_storage::RedisComm::GenaratePushMsgID(msg_id);
		std::string summary;
		current_time = time(NULL);
		OffLineMessage(private_send->platform_id,msg_id,current_time,send_user_info,
				recv_user_info,private_send->content,summary);
		PushMessage(private_send->platform_id,send_user_info,recv_user_info,summary);

	}else {
		chat_storage::RedisComm::GenaratePushMsgID(msg_id);
		//查询是否是一个会话，如果不是放入redis 聊天的session 和用户所在会话session 匹配
		LOG_DEBUG2("recv_user id %lld session :%lld chat session :%lld",
				recv_user_info.user_id(),recv_user_info.session(),private_send->session);
		if(recv_user_info.session()!=private_send->session){
			current_time = time(NULL);
			std::string summary;
			OffLineMessage(private_send->platform_id,msg_id,current_time,send_user_info,recv_user_info,private_send->content,summary);
		}else{
			struct TextChatPrivateRecv private_recv;
			MAKE_HEAD(private_recv, TEXT_CHAT_PRIVATE_RECV,CHAT_TYPE,0,private_send->reserverd);
			private_recv.platform_id = private_send->platform_id;
			private_recv.send_user_id = private_send->send_user_id;
			private_recv.recv_user_id = private_send->recv_user_id;
			private_recv.content = private_send->content;
			private_recv.msg_id = msg_id;
			current_time = private_recv.current_time;
			//sendmessage(recv_user_info.socket(),&private_recv);
			logic::SomeUtils::SendMessage(recv_user_info.socket(),&private_recv,__FILE__,__LINE__);
			pc->AddConfirmMessage(private_send->platform_id,private_recv.msg_id,send_user_info);
		}

	}

	// Leave Meassage
	return LeaveMessage(private_send->platform_id,msg_id,ALONE_CHAT,
			private_send->recv_user_id,time(NULL),send_user_info,
			private_send->content);
}

bool IMSMgr::OffLineMessage(const int64 platform_id,const int64 msg_id,const time_t current_time,
		const chat_base::UserInfo& send_userinfo,
		const chat_base::UserInfo& recv_userinfo,
		const std::string& message,std::string& summary){

	std::stringstream  s_send_uid;
	std::stringstream s_recv_uid;
	std::string detail;
	std::string s_current_time;
	double distance;

	chat_logic::LogicUnit::GetCurrentTimeFormat(current_time,s_current_time);

	s_send_uid<<send_userinfo.user_id();
	s_recv_uid<<recv_userinfo.user_id();

	chat_logic::LogicUnit::MakeLeaveContent(send_userinfo,recv_userinfo,
			message,TYPE_TEXT,summary,distance);
	/*修改存入数据库*/

	chat_storage::DBComm::RecordUserMessageList(MESSAGE_TYPE,send_userinfo.user_id(),
			recv_userinfo.user_id(),distance,summary);


	return true;
}

bool IMSMgr::LeaveMessage(const int64 platform_id,const int64 msg_id,const int32 type,
		const int64 oppid,const time_t current_time,const chat_base::UserInfo& send_userinfo,
		const std::string& message){
	std::string s_current_time;
	chat_logic::LogicUnit::GetCurrentTimeFormat(current_time,s_current_time);
	chat_base::MessageInfos messageinfos(platform_id,msg_id,send_userinfo.user_id(),oppid,
			type,send_userinfo.nickname(),message,s_current_time,send_userinfo.head_url());

	chat_storage::DBComm::RecordChatMessage(type,messageinfos);
	return true;
}

bool IMSMgr::LeaveMessage(const int64 platform_id,const int64 msg_id,const time_t current_time,
		const chat_base::UserInfo& send_userinfo,const chat_base::UserInfo& recv_userinfo,
		const std::string& message){

	std::string s_current_time;

	chat_logic::LogicUnit::GetCurrentTimeFormat(current_time,s_current_time);
	return chat_storage::DBComm::RecordMessage(platform_id,send_userinfo.user_id(),recv_userinfo.user_id(),
												msg_id,message,s_current_time);
}

// push to apple message center
bool IMSMgr::PushMessage(const int64 platform_id,const chat_base::UserInfo& send_userinfo,
		const chat_base::UserInfo& recv_userinfo,const std::string& message){
	//写入推送消息队列
	std::string name = "miyo";
	scoped_ptr<base_queue::BlockMsg>  msglist(new base_queue::BlockMsg());
	msglist->SetFormate(base_queue::TYPE_JSON);
	msglist->SetName(name);
	msglist->SetMsgType(2);

	scoped_ptr<base_queue::BlockMsg>  msg(new base_queue::BlockMsg());
	msg->SetBigInteger(L"tid",recv_userinfo.user_id());
	msg->SetString(L"message",message);
	msg->SetBigInteger(L"uid",send_userinfo.user_id());

	msglist->AddBlockMsg(msg.release());


	base_logic::WholeManager::GetWholeManager()->AddBlockMsgQueue(msglist->release());

	return true;

	/*std::stringstream os;
	std::string device_token;
	bool is_receive;
	unsigned begin_time;
	unsigned end_time;
	os<<send_userinfo.nickname()<<":"<<message;
	//get decivce token and  begin_time/end_time
	chat_storage::RedisComm::GetUserPushConfig(recv_userinfo.user_id(),
			device_token,is_receive,begin_time,end_time);

	return chat_logic::HttpComm::PushMessage(device_token,os.str());
	*/
}

}

