#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "basic/template.h"
#include <sstream>


//chl_r1 mm_r1 ms_r1
template <typename MapType,typename MapTypeIT>
static bool GetTypeRamdonTemplate(MapType &map, int idx,int* rands,int num){
	//
	MapTypeIT it = map.find(idx);
	if (it!=map.end()){
		it->second->GetPrize(rands,num);
		return true;
	}
	return false;
}

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
		//创建随机数
		pc->radom_in_.reset(new base::MigRadomIn());
		platform_cache_[platform_id] = pc;
	}
	pc->platform_info_ = platform;
	robot_storage::DBComm::GetRobotInfos(0,10000,pc->idle_robot_infos_);
	LOG_DEBUG2("pc->idle_robot_infos %ld",pc->idle_robot_infos_.size());
	RestMusicListRandom(pc);
}


bool RobotCacheManager::GetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform){
	logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	platform = pc->platform_info_;
	return true;
}

void RobotCacheManager::RestMusicListRandom(PlatformCache* pc){
	bool r = false;
	std::list<int> channel_list;
	std::list<int> mood_list;
	std::list<int> scene_list;
	r = robot_storage::DBComm::GetChannelInfos(channel_list);
	if (!r)
		assert(0);
	r = robot_storage::DBComm::GetMoodInfos(mood_list);
	if (!r)
		assert(0);
	r = robot_storage::DBComm::GetSceneInfos(scene_list);
	if (!r)
		assert(0);

	std::string chl = "chl";
	std::string mm = "mm";
	std::string ms = "ms";
	CreateTypeRamdon(pc,chl,channel_list);
	CreateTypeRamdon(pc,mm,mood_list);
	CreateTypeRamdon(pc,ms,scene_list);
}

bool RobotCacheManager::GetModeRadomSong(const int64 platform_id,const std::string& type,const int32& type_id,
		int num,std::list<int>& list){
	logic::RLockGd lk(lock_);
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	//获取随机歌曲
	GetTypeRamdon(pc,type,type_id,num,list);
	return true;
}

bool RobotCacheManager::GetIdleRobot(const int64 platform_id,const int64 uid,const double latitude,const double longitude,
		 std::list<robot_base::RobotBasicInfo>& list){
	logic::WLockGd lk(lock_);
	int i = 2;
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	while(i>0){
		r = GetRobot(latitude,longitude,pc->idle_robot_infos_,pc->temp_robot_infos_,list,pc->radom_in_.get());
		i--;
		if(!r)
			return r;
	}
	return true;
}

bool RobotCacheManager::RobotLoginSucess(const int64 platform_id,const int64 robot_uid,const int socket,const int64 uid){
	logic::WLockGd lk(lock_);
	robot_base::RobotBasicInfo robot_info;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	//删除临时表机器人，放入正式表
	bool r = base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->temp_robot_infos_,robot_uid,robot_info);
	if(!r)
		return false;
	robot_info.set_socket(socket);
	robot_info.set_follow_uid(uid);
	base::MapDel<RobotInfosMap,RobotInfosMap::iterator>(pc->temp_robot_infos_,robot_uid);
	base::MapAdd(pc->used_robot_infos_,robot_uid,robot_info);
	//更新机器人坐标
	robot_storage::DBComm::UpdateRobotLbsPos(robot_uid,robot_info.latitude(),robot_info.longitude());
	return AddUserFollowRobot(pc->user_follow_infos_,uid,robot_info);
}

bool RobotCacheManager::GetUserFollower(const int64 platform_id,const int64 uid,RobotInfosMap& robotinfos){
	logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	bool r = base::MapGet<UserFollowMap,UserFollowMap::iterator,RobotInfosMap>(pc->user_follow_infos_,uid,robotinfos);
	return r;
}

bool RobotCacheManager::GetUserFollowTaskRobot(const int64 platform_id,const int64 uid,const int32 task,robot_base::RobotBasicInfo& robotinfo){
	logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	RobotInfosMap robotinfos;
	bool r = base::MapGet<UserFollowMap,UserFollowMap::iterator,RobotInfosMap>(pc->user_follow_infos_,uid,robotinfos);
	if(!r)
		return false;
	GetTaskRobot(robotinfos,task,robotinfo);
	return true;
}

bool RobotCacheManager::SetScheduler(const int64 platform_id,robot_base::SchedulerInfo& scheduler_info){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapAdd<SchedulerMap,robot_base::SchedulerInfo>(pc->schduler_infos_,scheduler_info.socket(),scheduler_info);
}

bool RobotCacheManager::GetScheduler(const int64 platform_id,const int socket,robot_base::SchedulerInfo& scheduler_info){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapGet<SchedulerMap,SchedulerMap::iterator,robot_base::SchedulerInfo>(pc->schduler_infos_,socket,scheduler_info);
}

bool RobotCacheManager::DeleteScheduler(const int64 platform_id,const int socket){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapDel<SchedulerMap,SchedulerMap::iterator>(pc->schduler_infos_,socket);
}

bool RobotCacheManager::SchedulerSendMessage(const int64 platform_id,struct PacketHead* packet){
	logic::WLockGd lk(lock_);
	robot_base::SchedulerInfo scheduler_info;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	bool r = GetIdleScheduler(pc->schduler_infos_,scheduler_info);
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


bool RobotCacheManager::GetRobot(const double latitude,const double longitude,RobotInfosMap& idle_robot,RobotInfosMap& temp_robot,
		std::list<robot_base::RobotBasicInfo>& list,base::MigRadomIn* radomin){
	RobotInfosMap::iterator it = idle_robot.begin();
	if(it!=idle_robot.end()){
		double robot_latitude = 0;
		double robot_longitude = 0;
		GetRobotLbsPos(radomin,latitude,longitude,robot_latitude,robot_longitude);
		//修改坐标
		it->second.set_latitude(robot_latitude);
		it->second.set_longitude(robot_longitude);
		list.push_back(it->second);
		//存入临时表中，待机器人登录成功后，放入运行表中
		temp_robot[it->first] = it->second;
		LOG_DEBUG2("id %lld",it->first);
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
	scheduler_info = scheduler;
	return true;
}

bool RobotCacheManager::AddUserFollowRobot(UserFollowMap& usr_follow,const int64 uid,
		const robot_base::RobotBasicInfo& robotinfo){
	RobotInfosMap robotinfos;
	bool r = base::MapGet<UserFollowMap,UserFollowMap::iterator,RobotInfosMap>(usr_follow,uid,robotinfos);
	robotinfos[robotinfo.uid()] = robotinfo;
	usr_follow[uid] = robotinfos;
	return base::MapAdd<UserFollowMap,RobotInfosMap>(usr_follow,uid,robotinfos);

}

bool RobotCacheManager::GetTaskRobot(RobotInfosMap& robot_map,const int32 task,robot_base::RobotBasicInfo& robotinfo){
	//暂时以总任务数比较，
	RobotInfosMap::iterator it = robot_map.begin();
	robot_base::RobotBasicInfo robot;
	robot = it->second;
	for(;it!=robot_map.end();++it){
		robot_base::RobotBasicInfo temp_robot;
		temp_robot = it->second;
		if(temp_robot.task_count()>robot.task_count()){
			robot = it->second;
		}
	}
	robotinfo = robot;
	return true;
}

bool RobotCacheManager::GetRobotLbsPos(base::MigRadomIn* radomin,const double& latitude,const double& longitude,
		double& robot_latitude,double& robot_longitude){
	// 120.07900 30.28630
	double  int_latitude = latitude * 10; //1200.7900
	double  int_longitude= longitude * 10; //302.8630

	int64 radom_in_latitude = radomin->GetPrize(); //8650
	int64 radom_in_longitude = radomin->GetPrize(); // 2345

	double a_latitude = int_latitude * 10000 + radom_in_latitude;
	double a_longitude = int_longitude * 10000 + radom_in_longitude;

	robot_latitude = a_latitude / 100000;
	robot_longitude = a_longitude /100000;

	return true;

}

bool RobotCacheManager::CreateTypeRamdon(PlatformCache* pc,std::string& type,
									  std::list<int> &list){
	while(list.size()>0){
		int id = list.front();
		list.pop_front();
		//
		std::stringstream os;
		os<<type.c_str()<<"_r"<<id;
		int list_size = robot_storage::RedisComm::GetHashSize(os.str());
		if (list_size<=0)
			continue;

		LOG_DEBUG2("name[%s] list_size[%d]",os.str().c_str(),list_size);

		base::MigRadomInV2* radomV2 = new base::MigRadomInV2((list_size));
		if (type=="chl")
			pc->channel_random_map_[id] = radomV2;
		else if (type=="mm")
			pc->mood_random_map_[id] = radomV2;
		else if (type=="ms")
			pc->scene_random_map_[id] = radomV2;
	}
}

bool RobotCacheManager::GetTypeRamdon(PlatformCache* pc,const std::string& type,const int32& wordid,
        int num,std::list<int>& list){
	int* rands = NULL;
	rands = new int[num];
	if (rands==NULL)
		return false;

	bool r = false;
	if (type=="chl")
		r = GetTypeRamdonTemplate<std::map<int,base::MigRadomInV2*>,
		std::map<int,base::MigRadomInV2*>::iterator >(pc->channel_random_map_,wordid,
		rands,num);

	else if (type=="mm")
		r = GetTypeRamdonTemplate<std::map<int,base::MigRadomInV2*>,
		std::map<int,base::MigRadomInV2*>::iterator >(pc->mood_random_map_,wordid,
		rands,num);

	else if (type=="ms")
		r = GetTypeRamdonTemplate<std::map<int,base::MigRadomInV2*> ,
		std::map<int,base::MigRadomInV2*>::iterator>(pc->scene_random_map_,wordid,
		rands,num);

	if (!r)
		return r;
	int i = 0;
	while(i<num){
		list.push_back(rands[i]);
		i++;
	}

	if (rands){
		delete[] rands;
		rands = NULL;
	}

	return r;
}

}
