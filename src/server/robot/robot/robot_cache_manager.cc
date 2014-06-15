#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "basic/template.h"

namespace robot_logic{


RobotCacheManager* CacheManagerOp::robot_cache_mgr_ = NULL;
CacheManagerOp* CacheManagerOp::cache_manager_op_ = NULL;

RobotCacheManager::RobotCacheManager(){
    InitThreadrw(&lock_);
}

RobotCacheManager::~RobotCacheManager(){
    DeinitThreadrw(lock_);
}

void RobotCacheManager::SetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc == NULL){
		pc = new PlatformCache;
		platform_cache_[platform_id] = pc;
	}
	pc->platform_info_ = platform;
	robot_storage::DBComm::GetRobotInfos(0,10000,pc->idle_robot_infos);
	LOG_DEBUG2("pc->idle_robot_infos %ld",pc->idle_robot_infos.size());
}

bool RobotCacheManager::GetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform){
	logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	platform = pc->platform_info_;
	return true;
}


bool RobotCacheManager::GetIdleRobot(const int64 platform_id,const int64 uid,
		                         std::list<robot_base::RobotBasicInfo>& list){
	logic::WLockGd lk(lock_);
	int i = 2;
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	while(i>0){
		r = GetRobot(pc->idle_robot_infos,pc->temp_robot_infos,list);
		if(!r)
			return r;
	}
	return true;
}

bool RobotCacheManager::SetScheduler(const int64 platform_id,robot_base::SchedulerInfo& scheduler_info){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapAdd<SchedulerMap,robot_base::SchedulerInfo>(pc->schduler_infos,scheduler_info.socket(),scheduler_info);
}

bool RobotCacheManager::GetScheduler(const int64 platform_id,const int socket,robot_base::SchedulerInfo& scheduler_info){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapGet<SchedulerMap,SchedulerMap::iterator,robot_base::SchedulerInfo>(pc->schduler_infos,socket,scheduler_info);
}

bool RobotCacheManager::DeleteScheduler(const int64 platform_id,const int socket){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapDel<SchedulerMap,SchedulerMap::iterator>(pc->schduler_infos,socket);
}

bool RobotCacheManager::SchedulerSendMessage(const int64 platform_id,struct PacketHead* packet){
	logic::WLockGd lk(lock_);
	robot_base::SchedulerInfo scheduler_info;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	bool r = GetIdleScheduler(pc->schduler_infos,scheduler_info);
	if(!r)
		return false;

	return sendmessage(scheduler_info.socket(),packet);
}

PlatformCache* RobotCacheManager::GetPlatformCache(int64 platform_id){
	std::map<int64,PlatformCache*>::iterator it
		= platform_cache_.find(platform_id);

	if(it == platform_cache_.end())
		return NULL;
	return it->second;
}


bool RobotCacheManager::GetRobot(RobotInfosMap& idle_robot,
		RobotInfosMap& temp_robot,
		std::list<robot_base::RobotBasicInfo>& list){
	RobotInfosMap::iterator it = idle_robot.begin();
	if(it!=idle_robot.end()){
		list.push_back(it->second);
		//存入临时表中，待机器人登录成功后，放入运行表中
		temp_robot[it->first] = it->second;
		idle_robot.erase(it);
		return true;
	}
	return false;
}

bool RobotCacheManager::GetIdleScheduler(SchedulerMap& schduler_infos,robot_base::SchedulerInfo& scheduler_info){
	if(schduler_infos.size()<=0)
		return false;
	SchedulerMap::iterator it = schduler_infos.begin();
	robot_base::SchedulerInfo scheduler;
	scheduler = it->second;
	it++;
	for(;it!=schduler_infos.end();it++){
		robot_base::SchedulerInfo temp_scheduler;
		temp_scheduler = it->second;
		if(scheduler.client_count()>temp_scheduler.client_count()){
			scheduler = it->second;
		}
	}
	return true;
}

}
