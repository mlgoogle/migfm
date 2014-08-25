#include "robot_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "config/config.h"
#include <assert.h>
namespace robot_logic{

RobotEngine* RobotEngine::engine_ = NULL;

RobotEngine::RobotEngine(){
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	robot_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	robot_storage::DBComm::Init(config->mysql_db_list_);
	robot_storage::RedisComm::Init(config->redis_list_);
	music_mgr_.reset(new robot_logic::MusicMgr());
	user_mgr_.reset(new robot_logic::UserMgr());
	util_mgr_.reset(new robot_logic::UtilMgr());
}

RobotEngine::~RobotEngine(){

}

RobotEngine* RobotEngine::GetInstance(){
	if (engine_==NULL){
		engine_ = new RobotEngine();
	}

	return engine_;
}

void RobotEngine::FreeInstance(){
	if (engine_){
		delete engine_;
		engine_ =NULL;
	}
}


bool RobotEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool RobotEngine::OnBroadcastConnect(struct server *srv, int socket,
									  void *data, int len){
    return true;
}

bool RobotEngine::OnBroadcastMessage(struct server *srv, int socket, void *msg, int len){
    return true;
}

bool RobotEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
    return true;
}

bool RobotEngine::OnTimeout(struct server *srv, char *id, int opcode, int time){

	return true;
}

bool RobotEngine::OnRobotClose(struct server *srv, int socket){
    return true;
}

bool RobotEngine::OnRobotConnect(struct server *srv, int socket){
    return true;
}

bool RobotEngine::OnRobotMessage(struct server *srv, int socket,
								   const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);
	if(type=="getnewmusic")
		music_mgr_->GetNewMusic(socket,packet);
	else if(type=="addnewmusic")
		music_mgr_->AddNewMusic(socket,packet);
	else if(type=="getmailinfo")
		user_mgr_->GetUserInfoMail(socket,packet);
	else if(type=="getspreadmail")
		util_mgr_->GetSpreadMail(socket,packet);
	else if(type=="getrobots")
		user_mgr_->GetRobotsInfo(socket,packet);
	else if(type=="updaterobotpic")
		user_mgr_->UpdateRobotsHeadUrl(socket,packet);
	else if(type=="getvailedurlmusic")
		music_mgr_->GetUrlVailedMusic(socket,packet);
	else if(type=="updatevailedurlmusic")
		music_mgr_->UpdateVailedMusic(socket,packet);
	else if(type=="getvailedlyric")
		music_mgr_->GetVailedLyric(socket,packet);
	else if(type=="updatelyric")
		music_mgr_->UpdateVailedLyric(socket,packet);

    return true;
}



}
