#ifndef _ROBOT_ROBOT_CACHE_MANAGER_H__
#define _ROBOT_ROBOT_CACHE_MANAGER_H__

#include "logic_unit.h"
#include "robot_basic_info.h"
#include "base/comm_head.h"
#include "base/thread_handler.h"
#include "base/thread_lock.h"
#include "basic/scoped_ptr.h"
#include "basic/radom_in.h"
#include <list>

namespace robot_logic{


class PlatformCache{
public:
	robot_base::PlatformInfo                                platform_info_;
	RobotInfosMap                                           idle_robot_infos_;
	RobotInfosMap                                           used_robot_infos_;
	RobotInfosMap                                           temp_robot_infos_;
	UserFollowMap                                           user_follow_infos_;
	SchedulerMap                                            schduler_infos_;
	scoped_ptr<base::MigRadomIn>                            radom_in_;
	std::map<int,base::MigRadomInV2*>                       channel_random_map_;
	std::map<int,base::MigRadomInV2*>                       mood_random_map_;
	std::map<int,base::MigRadomInV2*>                       scene_random_map_;
	RobotInfosMap                                           assistant_;
};


class RobotCacheManager{
public:
	RobotCacheManager();
	virtual ~RobotCacheManager();
public:
	void SetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform);

	bool GetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform);

	bool GetIdleRobot(const int64 platform_id,const int64 uid,const double latitude,const double longitude,
			 std::list<robot_base::RobotBasicInfo>& list);

	bool GetIdleAssistant(const int64 platform_id,robot_base::RobotBasicInfo& assistant);

	bool NoticeAssistantLogin(const int64 platform_id);

	bool GetAssistantInfo(const int64 platform_id,const int64& uid,robot_base::RobotBasicInfo& assistant);

	bool GetUserFollower(const int64 platform_id,const int64 uid,RobotInfosMap& robotinfos);

	bool SetScheduler(const int64 platform_id,robot_base::SchedulerInfo& scheduler_info);

	bool GetScheduler(const int64 platform_id,const int socket,robot_base::SchedulerInfo& scheduler_info);

	bool DeleteScheduler(const int64 platform_id,const int socket);

	bool SchedulerSendMessage(const int64 platform_id,struct PacketHead* packet);

	bool RobotLoginSucess(const int64 platform_id,const int64 robot_uid,const int socket,const int64 uid,
			robot_base::RobotBasicInfo& robot);

	bool GetUserFollowTaskRobot(const int64 platform_id,const int64 uid,const int32 task,robot_base::RobotBasicInfo& robotinfo);

	bool GetUserFollowAllRobot(const int64 platform_id,const int64 uid,RobotInfosMap& map);

	bool ClearRobot(const int64 platform_id,const robot_base::RobotBasicInfo& robotinfo);

	void RestMusicListRandom(PlatformCache* pc);

	bool GetModeRadomSong(const int64 platform_id,const std::string& type,const int32& type_id,
			int num,std::list<int64>& list);

	void CheckRobotConnect(const int64 platform_id);


	void Dump();

private://内置函数，自身不能加锁

	void CreateTypeRamdon(PlatformCache* pc,std::string& type,std::list<int64>& list);

	bool GetTypeRamdon(PlatformCache* pc,const std::string& type,const int32& wordid,
	        int num,std::list<int64>& list);

	bool GetRobotLbsPos(base::MigRadomIn* radomin,const double& latitude,const double& longitude,double& robot_latitude,
			double& robot_longitude);

	bool GetRobot(const double latitude,const double longitude,RobotInfosMap& idle_robot,RobotInfosMap& temp_robot,
				std::list<robot_base::RobotBasicInfo>& list,base::MigRadomIn* radomin);
	bool GetIdleScheduler(SchedulerMap& schduler_infos,robot_base::SchedulerInfo& scheduler_info);

	bool AddUserFollowRobot(UserFollowMap& usr_follow,const int64 uid,const robot_base::RobotBasicInfo& robotinfo);

	bool GetTaskRobot(RobotInfosMap& robot_map,const int32 task,robot_base::RobotBasicInfo& robotinfo);



private:
	std::map<int64,PlatformCache*>             platform_cache_;
	struct threadrw_t*                         lock_;
	PlatformCache *GetPlatformCache (int64 platform_id);
};


class CacheManagerOp{
private:
	static RobotCacheManager  *robot_cache_mgr_;
	static CacheManagerOp     *cache_manager_op_;

public:
	CacheManagerOp();
	virtual ~CacheManagerOp();

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

public:
	bool SetRobotInfo(const int socket,const robot_base::RobotBasicInfo& robotinfo);
	bool GetRobotInfo(const int socket,robot_base::RobotBasicInfo& robotinfo);
	bool DelRobotInfo(const int socket);
private:
	SocketRobotInfosMap                        socket_robot_map_;
	struct threadrw_t*                         lock_;


};

}

#endif
