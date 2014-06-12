#include "robot_manager.h"
/*
#include "logic_unit.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "base/logic_comm.h"
#include "basic/radom_in.h"
#include "config/config.h"
#include "common.h"
*/
namespace robot_logic{


RobotManager::RobotManager(){

	if(!Init())
		assert(0);
	/*
	InitDefaultPlatformInfo();
	*/
}

RobotManager::~RobotManager(){
	base::SysRadom::GetInstance()->DeinitRandom();
}


bool RobotManager::InitDefaultPlatformInfo(){
	/*
	int64 platform_id = 10000;
	std::string platform_name = "miu";
	chat_base::PlatformInfo platforminfo(platform_id,platform_name);
	CacheManagerOp::GetPlatformChatMgrCache()->SetPlatformInfo(platform_id,platforminfo);
	*/
	return true;
}

bool RobotManager::Init(){

	/*
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}

	r = config->LoadConfig(path);
	chat_storage::DBComm::Init(config->mysql_db_list_);
	chat_storage::MemComm::Init(config->mem_list_);
	chat_storage::RedisComm::Init(config->redis_list_);

	usr_connection_mgr_.reset(new chat_logic::UserConnectionMgr());
	ims_mgr_.reset(new chat_logic::IMSMgr());
	file_mgr_.reset(new chat_logic::FileMgr());
	base::SysRadom::GetInstance()->InitRandom();
*/
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
   /* bool r = false;
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
	*/
    
    return true;
}

bool RobotManager::OnRobotManagerClose(struct server *srv,
									 const int socket){

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
	return true;
}

bool RobotManager::OnTimeout(struct server *srv, char *id,
							int opcode, int time){
    return true;
}

}
