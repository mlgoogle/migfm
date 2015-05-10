#include "chat_logic.h"
#include "db_comm.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "common.h"

namespace chatsvc_logic{

Chatlogic*
Chatlogic::instance_=NULL;

Chatlogic::Chatlogic(){
   if(!Init())
      assert(0);
}

Chatlogic::~Chatlogic(){
	chatsvc_logic::DBComm::Dest();
}

bool Chatlogic::Init(){
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}
	r = config->LoadConfig(path);
	chatsvc_logic::DBComm::Init(config->mysql_db_list_);
    return true;
}

Chatlogic*
Chatlogic::GetInstance(){

    if(instance_==NULL)
        instance_ = new Chatlogic();

    return instance_;
}



void Chatlogic::FreeInstance(){
    delete instance_;
    instance_ = NULL;
}

bool Chatlogic::OnChatConnect(struct server *srv,const int socket){

    return true;
}



bool Chatlogic::OnChatMessage(struct server *srv, const int socket, const void *msg,const int len){
	const char* packet_stream = (char*)(msg);
	std::string http_str(packet_stream,len);
	std::string error_str;
	int error_code = 0;
	LOG_DEBUG2("%s",packet_stream);

	scoped_ptr<base_logic::ValueSerializer> serializer(base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP,&http_str));


	netcomm_recv::NetBase*  value = (netcomm_recv::NetBase*)(serializer.get()->Deserialize(&error_code,&error_str));
	if(value==NULL){
		error_code = STRUCT_ERROR;
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	scoped_ptr<netcomm_recv::HeadPacket> packet(new netcomm_recv::HeadPacket(value));
	int32 type = packet->GetType();
	switch(type){
	 case CHAT_GAIN_GROUP_MESSAGE:
		 OnGainGroupMessage(srv,socket,value);
		 break;
	 case CHAT_GAIN_ALONE_MESSAGE:
		 OnGainLeaveMessage(srv,socket,value);
		 break;
	 case CHAT_GAIN_IDLE_CHAT_SERVER:
		 OnGainIdleChatSvc(srv,socket,value);
		 break;
	}
    return true;
}

bool Chatlogic::OnChatClose(struct server *srv,const int socket){

    return true;
}



bool Chatlogic::OnBroadcastConnect(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}

bool Chatlogic::OnBroadcastMessage(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}



bool Chatlogic::OnBroadcastClose(struct server *srv, const int socket){

    return true;
}

bool Chatlogic::OnIniTimer(struct server *srv){

    return true;
}



bool Chatlogic::OnTimeout(struct server *srv, char *id, int opcode, int time){

    return true;
}

bool Chatlogic::OnGainGroupMessage(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
        		const void* msg,const int len){
	scoped_ptr<netcomm_recv::GroupMessage> group_message(new netcomm_recv::GroupMessage(netbase));
	std::list<chatsvc_logic::GroupMessageInfos> list;
	int error_code = group_message->GetResult();
	int64 group_id = 0;
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}

	//读取聊天记录
	scoped_ptr<netcomm_send::GroupMessage> send_groupmsg(new netcomm_send::GroupMessage());
	bool r = chatsvc_logic::DBComm::OnGetGroupMessage(group_message->platform(),
			group_message->groupid(),group_message->from(),group_message->count(),
			group_message->msgid(),list);
	while(r&&list.size()>0){
		chatsvc_logic::GroupMessageInfos message = list.front();
		list.pop_front();
		send_groupmsg->set_message(message.platform(),message.msgid(),
				message.fid(),message.groupid(),message.nickname(),
				message.head(),
				message.message(),message.lasttime());
	}
	//发送
	send_message(socket,(netcomm_send::HeadPacket*)send_groupmsg.get());
	return true;
}

bool Chatlogic::OnGainLeaveMessage(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
      		const void* msg,const int len ){
	scoped_ptr<netcomm_recv::AloneMessage> alone_message(new netcomm_recv::AloneMessage(netbase));
	std::list<chatsvc_logic::AloneMessageInfos> list;
	int error_code = alone_message->GetResult();
	int64 group_id = 0;
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}

	//读取聊天记录
	scoped_ptr<netcomm_send::AloneMessage> send_alonemsg(new netcomm_send::AloneMessage());
	bool r = chatsvc_logic::DBComm::OnGetLeaveMessage(alone_message->platform(),
			alone_message->fid(),alone_message->tid(),alone_message->from(),
			alone_message->count(),
			alone_message->msgid(),list);
	while(r&&list.size()>0){
		chatsvc_logic::AloneMessageInfos message = list.front();
		list.pop_front();
		send_alonemsg->set_message(message.platform(),message.msgid(),
				message.fid(),message.tid(),message.nickname(),
				message.head(),
				message.message(),message.lasttime());
	}
	//发送
	send_message(socket,(netcomm_send::HeadPacket*)send_alonemsg.get());
	return true;
}

bool Chatlogic::OnGainIdleChatSvc(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
         		const void* msg,const int len){
	scoped_ptr<netcomm_recv::GetIdleChat> idle_chat(new netcomm_recv::GetIdleChat(netbase));
	int error_code = idle_chat->GetResult();
	int64 group_id = 0;
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	scoped_ptr<netcomm_send::GetIdleChat> sidle_chat(new netcomm_send::GetIdleChat());
	sidle_chat->SetHost("112.124.49.59",17000);
	//发送
	send_message(socket,(netcomm_send::HeadPacket*)sidle_chat.get());
	return true;
}

}

