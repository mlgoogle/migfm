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
	return true;
}

bool SchedulerMgr::OnDisQuiteScheduler(const int socket){
	return robot_logic::CacheManagerOp::GetRobotCacheMgr()->DeleteScheduler(1000,socket);
}

}
