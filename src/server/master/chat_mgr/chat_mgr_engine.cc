#include "chat_mgr_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "basic/errno_comm.h"
#include "config/config.h"
#include "basic/radom_in.h"
#include "storage/dic_storage.h"
#include <sstream>

#define		TIME_TEST		1025

namespace chat_logic{


ChatMgrEngine::ChatMgrEngine(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	chat_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	chat_storage::DBComm::Init(config->mysql_db_list_);
	chat_storage::MemComm::Init(config->mem_list_);

	message_mgr_.reset(new chat_logic::MessageMgr());
	srv_mgr_.reset(new chat_logic::ServerMgr());
}

ChatMgrEngine::~ChatMgrEngine(){
	chat_logic::ThreadKey::DeinitThreadKey ();
}

ChatMgrEngine* ChatMgrEngine::instance_ = NULL;
ChatMgrEngine* ChatMgrEngine::GetInstance(){
   if (instance_==NULL){
        instance_ = new ChatMgrEngine();
   }

   return instance_;
}

void ChatMgrEngine::FreeInstance(){
	delete instance_;
}

bool ChatMgrEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool ChatMgrEngine::OnBroadcastConnect(struct server *srv, int socket,
									  void *data, int len){
    return true;
}

bool ChatMgrEngine::OnBroadcastMessage(struct server *srv, int socket, void *msg, int len){
    return true;
}

bool ChatMgrEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
    return true;
}

bool ChatMgrEngine::OnTimeout(struct server *srv, char *id, int opcode, int time){

	return true;
}

bool ChatMgrEngine::OnChatMgrClose(struct server *srv, int socket){
    return true;
}

bool ChatMgrEngine::OnChatMgrConnect(struct server *srv, int socket){
    return true;
}

bool ChatMgrEngine::OnChatMgrMessage(struct server *srv, int socket,
								   const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);

	if (type=="getsc"){
		srv_mgr_->GetBestIdle(socket,packet);
	}else if(type=="hischat"){
		message_mgr_->GetLeaveMessage(socket,packet);
	}else if(type=="getsc1"){
		srv_mgr_->GetBestIdleTest(socket,packet);
	}
    return true;
}


}
