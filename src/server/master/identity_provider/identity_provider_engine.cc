#include "identity_provider_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include <sstream>

#define		TIME_TEST		1025

namespace sso_logic{


IdentityProviderEngine::IdentityProviderEngine(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	sso_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	storage::DBComm::Init(config->mysql_db_list_);
	storage::MemComm::Init(config->mem_list_);
	storage::RedisComm::Init(config->redis_list_);

}

IdentityProviderEngine::~IdentityProviderEngine(){
}

IdentityProviderEngine* IdentityProviderEngine::instance_ = NULL;
IdentityProviderEngine* IdentityProviderEngine::GetInstance(){
   if (instance_==NULL){
        instance_ = new IdentityProviderEngine();
   }

   return instance_;
}

void IdentityProviderEngine::FreeInstance(){
	delete instance_;
}

bool IdentityProviderEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool IdentityProviderEngine::OnBroadcastConnect(struct server *srv, int socket, 
									  void *data, int len){
    return true;
}

bool IdentityProviderEngine::OnBroadcastMessage(struct server *srv, int socket, void *msg, int len){
    return true;
}

bool IdentityProviderEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
    return true;
}

bool IdentityProviderEngine::OnTimeout(struct server *srv, 
										 char *id, int opcode, int time){

	return true;
}

bool IdentityProviderEngine::OnClose(struct server *srv, int socket){
    return true;
}

bool IdentityProviderEngine::OnConnect(struct server *srv, int socket){
    return true;
}

bool IdentityProviderEngine::OnMessage(struct server *srv, int socket, 
						     const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);
    return true;
}

}