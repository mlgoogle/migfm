#include "scheduler_mgr.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "base/comm_head.h"

namespace robot_logic{

SchedulerMgr::SchedulerMgr(){

}

SchedulerMgr::~SchedulerMgr(){

}

bool SchedulerMgr::OnSchedulerMgrLogin(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg, int len){

	struct SchedulerLogin* vSchedulerLogin = (struct SchedulerLogin*)packet;
	std::string host;
	robot_base::SchedulerInfo schduler(vSchedulerLogin->platform_id,socket,host,vSchedulerLogin->machine_id);
	robot_logic::CacheManagerOp::GetRobotCacheMgr()->SetScheduler(vSchedulerLogin->platform_id,schduler);
	//通知助理机器人登录
	robot_logic::CacheManagerOp::GetRobotCacheMgr()->NoticeAssistantLogin(vSchedulerLogin->platform_id);
	return true;
}

bool SchedulerMgr::OnDisQuiteScheduler(const int socket){
	return robot_logic::CacheManagerOp::GetRobotCacheMgr()->DeleteScheduler(10000,socket);
}

bool SchedulerMgr::NoticeAssistant(){
	return robot_logic::CacheManagerOp::GetRobotCacheMgr()->NoticeAssistantLogin(10000);
}

bool SchedulerMgr::OnNoticeAssistant(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg, int len){
	robot_base::RobotBasicInfo robot_info;
	struct AssistantLoginSuccess * assistant_login_success = (struct AssistantLoginSuccess*)packet;
	bool r = robot_logic::CacheManagerOp::GetRobotCacheMgr()->SetAssistantLogin(assistant_login_success->platform_id,
	assistant_login_success->assistant_id,socket);

	//bool r = robot_logic::CacheManagerOp::GetRobotCacheMgr()->GetAssistantInfo(assistant_login_success->platform_id,
		//	assistant_login_success->assistant_id,robot_info);
	//robot_info.set_login_status(1);
	//robot_info.set_socket(socket);
	return true;
}

}
