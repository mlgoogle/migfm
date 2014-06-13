#ifndef _ROBOT_ROBOT_CACHE_MANAGER_H__
#define _ROBOT_ROBOT_CACHE_MANAGER_H__

#include "logic_unit.h"
#include "robot_basic_info.h"
#include "base/thread_handler.h"
#include "base/thread_lock.h"

namespace robot_logic{


class PlatformCache{
public:
	robot_base::PlatformInfo   platform_info_;
	RobotInfosMap    idle_robot_infos;
	RobotInfosMap    used_robot_infos;
};


class RobotCacheManager{
public:
	RobotCacheManager();
	virtual ~RobotCacheManager();
public:
	void SetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform);

	bool GetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform);




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
