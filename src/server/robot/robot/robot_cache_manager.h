#ifndef _ROBOT_ROBOT_CACHE_MANAGER_H__
#define _ROBOT_ROBOT_CACHE_MANAGER_H__

#include "logic_unit.h"
#include "robot_basic_info.h"
#include "base/comm_head.h"
#include "base/thread_handler.h"
#include "base/thread_lock.h"
#include <list>

namespace robot_logic{


class PlatformCache{
public:
	robot_base::PlatformInfo   platform_info_;
	RobotInfosMap    idle_robot_infos;
	RobotInfosMap    used_robot_infos;
	RobotInfosMap    temp_robot_infos;
	UserFollowMap    user_follow_infos;
	SchedulerMap     schduler_infos;
};


class RobotCacheManager{
public:
	RobotCacheManager();
	virtual ~RobotCacheManager();
public:
	void SetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform);

	bool GetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform);

	bool GetIdleRobot(const int64 platform_id,const int64 uid,std::list<robot_base::RobotBasicInfo>& list);

	bool SetScheduler(const int64 platform_id,robot_base::SchedulerInfo& scheduler_info);

	bool GetScheduler(const int64 platform_id,const int socket,robot_base::SchedulerInfo& scheduler_info);

	bool DeleteScheduler(const int64 platform_id,const int socket);

	bool SchedulerSendMessage(const int64 platform_id,struct PacketHead* packet);

	bool RobotLoginSucess(const int64 platform_id,const int64 robot_uid,const int socket,const int64 uid);

private://内置函数，自身不能加锁
	bool GetRobot(RobotInfosMap& idle_robot,RobotInfosMap& temp_robot,
				std::list<robot_base::RobotBasicInfo>& list);
	bool GetIdleScheduler(SchedulerMap& schduler_infos,robot_base::SchedulerInfo& scheduler_info);

	bool AddUserFollowRobot(UserFollowMap& usr_follow,const int64 uid,const robot_base::RobotBasicInfo& robotinfo);

private:
	std::map<int64,PlatformCache*>             platform_cache_;
	struct threadrw_t*                         lock_;
	PlatformCache *GetPlatformCache (int64 platform_id);
};


class CacheManagerOp{
private:
	static RobotCacheManager  *robot_cache_mgr_;
	static CacheManagerOp     *cache_manager_op_;


	CacheManagerOp() {}
	virtual ~CacheManagerOp() {}
public:
	static RobotCacheManager* GetRobotCacheMgr(){
		if (robot_cache_mgr_ == NULL)
			robot_cache_mgr_ = new RobotCacheManager ();

		return robot_cache_mgr_;
	}

	static void FreeRobotCacheMgr (){
		delete robot_cache_mgr_;
	}

	static CacheManagerOp* GetCacheManagerOp(){
		if(cache_manager_op_ == NULL){
			cache_manager_op_ = new CacheManagerOp();
		}
		return cache_manager_op_;
	}

	static void FreeCacheManagerOp(){
		delete cache_manager_op_;
	}


};
}

#endif
