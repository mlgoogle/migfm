#include "robot_manager.h"
#include "robot_cache_manager.h"
#include "logic_unit.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "base/logic_comm.h"
#include "basic/radom_in.h"
#include "config/config.h"
#include "common.h"


#define TIME_CHECK_CONNECT    10025//检测连接
#define TIME_CHECK_WEATCHER   10026//检测天气
namespace robot_logic{


RobotManager::RobotManager(){

	if(!Init())
		assert(0);
	InitDefaultPlatformInfo();
}

RobotManager::~RobotManager(){
	base::SysRadom::GetInstance()->DeinitRandom();
}


bool RobotManager::InitDefaultPlatformInfo(){
	int64 platform_id = 10000;
	std::string platform_name = "miu";
	robot_base::PlatformInfo platforminfo(platform_id,platform_name);
	CacheManagerOp::GetRobotCacheMgr()->SetPlatformInfo(platform_id,platforminfo);
	return true;
}

bool RobotManager::Init(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}

	r = config->LoadConfig(path);
	robot_storage::DBComm::Init(config->mysql_db_list_);
	robot_storage::RedisComm::Init(config->redis_list_);
	scheduler_mgr_.reset(new robot_logic::SchedulerMgr());
	robot_mgr_.reset(new robot_logic::RobotConnection());
	robot_song_mgr_.reset(new robot_logic::RobotSongMgr());
	robot_weather_mgr_.reset(new robot_logic::RobotWeatherMgr());
	/*chat_storage::MemComm::Init(config->mem_list_);*/

/*
	usr_connection_mgr_.reset(new chat_logic::UserConnectionMgr());
	ims_mgr_.reset(new chat_logic::IMSMgr());
	file_mgr_.reset(new chat_logic::FileMgr());
	base::SysRadom::GetInstance()->InitRandom();
*/
	robot_weather_mgr_.get()->OnPullWeatherInfo();
	return true;
}

RobotManager* RobotManager::instance_ = NULL;

RobotManager*
RobotManager::GetInstance(){
    
	if (instance_==NULL)
		instance_ = new RobotManager;
	return instance_;
}


void RobotManager::FreeInstance(){
	delete instance_;
	instance_ = NULL;
}


bool RobotManager::OnRobotConnect(struct server *srv,
								const int socket){
    return true;
}

bool RobotManager::OnRobotManagerMessage(struct server *srv,
										 const int socket, 
										 const void *msg, 
										 const int len){
    bool r = false;
	struct PacketHead *packet = NULL;
	if (srv == NULL
		||socket<=0
		||msg == NULL
		||len < PACKET_HEAD_LENGTH)
		return false;

	if (!ProtocolPack::UnpackStream(msg,len,&packet)){
		struct PacketHead *ph = (struct PacketHead*) msg;
		LOG_ERROR2("Call UnpackStream failed.operate_code %d packet_length %d len %d",
			ph->operate_code,ph->packet_length,len);
		return false;
	}

	assert (packet);
	LOG_DEBUG2("packet->packet_length[%d],packet->packet_operate[%d],packet->data_length[%d]",
			packet->packet_length,packet->operate_code,packet->data_length);

	//ProtocolPack::HexEncode(msg,len);
	ProtocolPack::DumpPacket(packet);
	switch(packet->operate_code){
	case SCHEDULER_LOGIN:
		scheduler_mgr_.get()->OnSchedulerMgrLogin(srv,socket,packet);
		break;
	case NOTICE_USER_LOGIN:
		robot_mgr_.get()->OnUserLogin(srv,socket,packet);
	case ROBOT_LOGIN:
		robot_mgr_.get()->OnRobotLogin(srv,socket,packet);
		robot_song_mgr_.get()->OnRobotLoginSong(srv,socket,packet);
		break;
	case NOTICE_USER_DEFAULT_SONG:
		robot_song_mgr_.get()->OnUserDefaultSong(srv,socket,packet);
		break;
	case NOTICE_USER_CURRENT_SONG:
		robot_song_mgr_.get()->OnNoticeUserChangerSong(srv,socket,packet);
	default:
		break;
	}
    
    return true;
}

bool RobotManager::OnRobotManagerClose(struct server *srv,
									 const int socket){
	robot_mgr_.get()->OnClearRobotConnection(socket);
    return true;
}

bool RobotManager::OnBroadcastConnect(struct server *srv,
									 const int socket, 
									 const void *data, 
									 const int len){
    return true;
}

bool RobotManager::OnBroadcastMessage(struct server *srv,
									 const int socket, 
									 const void *msg, 
									 const int len){
    return true;
}

bool RobotManager::OnBroadcastClose(struct server *srv,
									const int socket){
    return true;
}

bool RobotManager::OnIniTimer(struct server *srv){
	//检测心跳包 发给机器人和控制器
	srv->add_time_task(srv,PLUGIN_ID,TIME_CHECK_CONNECT,20,-1);
	return true;
}

bool RobotManager::OnTimeout(struct server *srv, char *id,
							int opcode, int time){
	switch(opcode){
	case TIME_CHECK_CONNECT:
			LOG_DEBUG2("TIME_CHECK_CONNECT :%d",TIME_CHECK_CONNECT);
			CacheManagerOp::GetRobotCacheMgr()->CheckRobotConnect(10000);
			CacheManagerOp::GetRobotCacheMgr()->Dump();
		break;
	default:
		LOG_ERROR2("unkown code :%d",opcode);
	}
    return true;
}


}
