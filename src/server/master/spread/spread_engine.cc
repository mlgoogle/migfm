#include "spread_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "basic/errno_comm.h"
#include "config/config.h"
#include "basic/radom_in.h"
#include "storage/dic_storage.h"
#include "json/json.h"
#include <sstream>

#define		TIME_TEST		1025

namespace spread_logic{


SpreadEngine::SpreadEngine(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	spread_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	spread_storage::DBComm::Init(config->mysql_db_list_);
	spread_storage::MemComm::Init(config->mem_list_);
	spread_storage::RedisComm::Init(config->redis_list_);

	weixin_mgr_.reset(new spread_logic::WeiXinMgr());

}

SpreadEngine::~SpreadEngine(){
	spread_logic::ThreadKey::DeinitThreadKey ();
}

SpreadEngine* SpreadEngine::instance_ = NULL;
SpreadEngine* SpreadEngine::GetInstance(){
   if (instance_==NULL){
        instance_ = new SpreadEngine();
   }

   return instance_;
}

void SpreadEngine::FreeInstance(){
	delete instance_;
}

bool SpreadEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool SpreadEngine::OnBroadcastConnect(struct server *srv, int socket,
									  void *data, int len){
    return true;
}

bool SpreadEngine::OnBroadcastMessage(struct server *srv, int socket, void *msg, int len){
    return true;
}

bool SpreadEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
    return true;
}

bool SpreadEngine::OnTimeout(struct server *srv, char *id, int opcode, int time){

	return true;
}

bool SpreadEngine::OnSpreadClose(struct server *srv, int socket){
    return true;
}

bool SpreadEngine::OnSpreadConnect(struct server *srv, int socket){
    return true;
}

bool SpreadEngine::OnSpreadMessage(struct server *srv, int socket,
								   const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);
	if(type=="weixin")
		weixin_mgr_->Message(socket,packet);
    return true;
}

}
