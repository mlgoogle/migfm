#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "basic/base64.h"
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
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc == NULL){
		pc = new PlatformCache;
		//创建随机数
		pc->radom_in_.reset(new base::MigRadomIn());
		platform_cache_[platform_id] = pc;
	}
	pc->platform_info_ = platform;
	robot_storage::DBComm::GetRobotInfos(0,10000,pc->idle_robot_infos_);
	robot_storage::DBComm::GetAssistant(pc->assistant_);
	LOG_DEBUG2("pc->idle_robot_infos %ld",pc->idle_robot_infos_.size());
	RestMusicListRandom(pc);
}


bool RobotCacheManager::GetPlatformInfo(const int64 platform_id,robot_base::PlatformInfo& platform){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	platform = pc->platform_info_;
	return true;
}

void RobotCacheManager::RestMusicListRandom(PlatformCache* pc){
	bool r = false;
	std::list<int64> channel_list;
	std::list<int64> mood_list;
	std::list<int64> scene_list;
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
		int num,std::list<int64>& list){
	robot_logic::RLockGd lk(lock_);
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
	robot_logic::WLockGd lk(lock_);
	int i = 2;
	bool r = false;
	RobotInfosMap  follow_robotinfos;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	//检测是否已经有了伴随机器人
	r = base::MapGet<UserFollowMap,UserFollowMap::iterator,RobotInfosMap>(pc->user_follow_infos_,uid,follow_robotinfos);
	if(r){//存在
		//重新计算坐标
		for(RobotInfosMap::iterator itr =  follow_robotinfos.begin();itr!=follow_robotinfos.end();++itr){
			double robot_latitude = 0;
			double robot_longitude = 0;
			GetRobotLbsPos(pc->radom_in_.get(),latitude,longitude,robot_latitude,robot_longitude);
			robot_base::RobotBasicInfo robot = itr->second;
			robot.set_latitude(robot_latitude);
			robot.set_longitude(robot_longitude);
			robot.set_follower_user_last_time(time(NULL));
			list.push_back(robot);
		}
	}else{//不存在
		while(i>0){
			r = GetRobot(latitude,longitude,pc->idle_robot_infos_,pc->temp_robot_infos_,list,pc->radom_in_.get());
			i--;
			if(!r)
				return r;
		}
	}
	return true;
}

bool RobotCacheManager::RobotLoginSucess(const int64 platform_id,const int64 robot_uid,const int socket,const int64 uid,
		robot_base::RobotBasicInfo& robot){
	robot_logic::WLockGd lk(lock_);
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
	robot_info.set_recv_last_time(time(NULL));
	robot_info.set_follower_user_last_time(time(NULL));
	base::MapDel<RobotInfosMap,RobotInfosMap::iterator>(pc->temp_robot_infos_,robot_uid);
	base::MapAdd<RobotInfosMap,robot_base::RobotBasicInfo>(pc->used_robot_infos_,robot_uid,robot_info);
	//更新机器人坐标
	robot_storage::DBComm::UpdateRobotLbsPos(robot_uid,robot_info.latitude(),robot_info.longitude());
	robot = robot_info;
	return AddUserFollowRobot(pc->user_follow_infos_,uid,robot_info);
}

bool RobotCacheManager::GetUserFollower(const int64 platform_id,const int64 uid,RobotInfosMap& robotinfos){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	bool r = base::MapGet<UserFollowMap,UserFollowMap::iterator,RobotInfosMap>(pc->user_follow_infos_,uid,robotinfos);
	return r;
}

bool RobotCacheManager::GetUserFollowTaskRobot(const int64 platform_id,const int64 uid,const int32 task,robot_base::RobotBasicInfo& robotinfo){
	robot_logic::RLockGd lk(lock_);
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

bool RobotCacheManager::GetUserFollowAllRobot(const int64 platform_id,const int64 uid,RobotInfosMap& map){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	RobotInfosMap robotinfos;
	bool r = base::MapGet<UserFollowMap,UserFollowMap::iterator,RobotInfosMap>(pc->user_follow_infos_,uid,map);
	if(!r)
		return false;
	return true;
}

bool RobotCacheManager::GetUsedRobotInfo(const int64 platform_id,const int64 uid,robot_base::RobotBasicInfo& robotinfo){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->used_robot_infos_,uid,robotinfo);
}

bool RobotCacheManager::GetIdelRobotInfo(const int64 platform_id,const int64 uid,robot_base::RobotBasicInfo& robotinfo){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->idle_robot_infos_,uid,robotinfo);
}

bool RobotCacheManager::ClearRobot(const int64 platform_id,const robot_base::RobotBasicInfo& robotinfo){
	robot_logic::WLockGd lk(lock_);
	std::map<int64,int64> follow_uid;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	//int64 uid = robotinfo.uid();
	follow_uid = robotinfo.follow_uid();
	if(follow_uid.size()<=0)
		return false;
	//清理对应的机器人信息
	for(std::map<int64,int64>::iterator it = follow_uid.begin();it!=follow_uid.end();++it){
		//获取用户信息
		int64 uid = it->first;

		//删除用户信息
		base::MapDel<UserInfoMap,UserInfoMap::iterator>(pc->user_infos_,uid);

		RobotInfosMap robotinfos;
		base::MapGet<UserFollowMap,UserFollowMap::iterator,RobotInfosMap>(pc->user_follow_infos_,uid,robotinfos);
		if(robotinfos.size()<=0)
			continue;
		//删除跟随机器人
		base::MapDel<RobotInfosMap,RobotInfosMap::iterator>(robotinfos,robotinfo.uid());

		//检测是否是最后一个
		if(robotinfos.size()==0)
			base::MapDel<UserFollowMap,UserFollowMap::iterator>(pc->user_follow_infos_,uid);
	}
	//从正在使用中放入空闲的底部
	base::MapDel<RobotInfosMap,RobotInfosMap::iterator>(pc->used_robot_infos_,robotinfo.uid());
	base::MapAdd<RobotInfosMap,robot_base::RobotBasicInfo>(pc->idle_robot_infos_,robotinfo.uid(),robotinfo);

	return true;
}

void RobotCacheManager::SetUserInfo(const int64 platform_id,const int64 uid,int current_weather,int future_weather,int future_time){
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return ;
	//获取用户是否存在
	robot_base::UserBasicInfo userinfo;
	bool r = base::MapGet<UserBasicMap,UserBasicMap::iterator,robot_base::UserBasicInfo>(pc->user_basic_infos_,uid,userinfo);
	if(!r){//不存在
		robot_base::UserBasicInfo temp_info(uid);
		userinfo = temp_info;
	}else{//存在
		//获取上个天气状态
		int last_weather_status = userinfo.last_weather_status();
		//判断是雨天还是非雨天
		if(last_weather_status!=future_weather)//雨天转非雨天，通知用户
			//放入队列
			return;
	}
	userinfo.set_last_weather_status(current_weather);
	base::MapAdd<UserBasicMap,robot_base::UserBasicInfo>(pc->user_basic_infos_,uid,userinfo);
}

bool RobotCacheManager::GetUserAddressInfo(const int64 platform_id,const int64 uid,std::string& city,
		std::string& district,std::string& province,std::string& street){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	robot_base::UserBasicInfo userinfo;
	if(pc==NULL)
		return false;
	bool r = base::MapGet<UserBasicMap,UserBasicMap::iterator,robot_base::UserBasicInfo>(pc->user_basic_infos_,uid,userinfo);
	if(!r)
		return false;
	city = userinfo.city();
	district = userinfo.distict();
	province = userinfo.province();
	street = userinfo.street();
	if(city.empty()||province.empty())
		return false;
	return true;
}

bool RobotCacheManager::SetUserAddressInfo(const int64 platform_id,const int64 uid,std::string& city,
		std::string& district,std::string& province,std::string& street){
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	robot_base::UserBasicInfo userinfo;
	if(pc==NULL)
		return false;
	bool r = base::MapGet<UserBasicMap,UserBasicMap::iterator,robot_base::UserBasicInfo>(pc->user_basic_infos_,uid,userinfo);
	if(!r)
		return false;
	userinfo.set_city(city);
	userinfo.set_distict(district);
	userinfo.set_province(province);
	userinfo.set_street(street);
	return true;
}

bool RobotCacheManager::IsPushMessageDay(const int64 platform_id,const int64 uid){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	robot_base::UserBasicInfo userinfo;
	if(pc==NULL)
		return false;
	bool r = base::MapGet<UserBasicMap,UserBasicMap::iterator,robot_base::UserBasicInfo>(pc->user_basic_infos_,uid,userinfo);
	if(!r)
		return false;
	//获取当前时间
	 time_t current = time(NULL);
	 struct tm* local = localtime(&current);
	 if((local->tm_mon+1)!=userinfo.push_month() || local->tm_mday!=userinfo.push_day())
		 return true;
	 return false;
}

bool RobotCacheManager::SetUserPushMessageDay(const int64 platform_id,const int64 uid){
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	robot_base::UserBasicInfo userinfo;
	if(pc==NULL)
		return false;
	bool r = base::MapGet<UserBasicMap,UserBasicMap::iterator,robot_base::UserBasicInfo>(pc->user_basic_infos_,uid,userinfo);
	if(!r)
		return false;
	//获取当前时间
	 time_t current = time(NULL);
	 struct tm* local = localtime(&current);
	 userinfo.set_push_month(local->tm_mon+1);
	 userinfo.set_push_day(local->tm_mday);
	 return base::MapAdd<UserBasicMap,robot_base::UserBasicInfo>(pc->user_basic_infos_,uid,userinfo);
}

bool RobotCacheManager::SetUserInfoLogin(const int64 platform_id,const int64 uid){
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	robot_base::UserInfo userinfo(uid);
	if(pc==NULL)
		return false;
	 return base::MapAdd<UserInfoMap,robot_base::UserInfo>(pc->user_infos_,uid,userinfo);
}

bool RobotCacheManager::SetUserListenState(const int64 platform_id,const int64 uid,const int32 type_id,
		const std::string& mode){
	bool r = false;
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	robot_base::UserInfo userinfo;
	if(pc==NULL)
		return false;
	r =  base::MapGet<UserInfoMap,UserInfoMap::iterator,robot_base::UserInfo>(pc->user_infos_,uid,userinfo);
	if(!r)
		return r;
	userinfo.set_mode(mode);
	userinfo.set_type_id(type_id);
	return base::MapAdd<UserInfoMap,robot_base::UserInfo>(pc->user_infos_,uid,userinfo);
}

bool RobotCacheManager::GetIdleAssistant(const int64 platform_id,robot_base::RobotBasicInfo& assistant){
	robot_logic::RLockGd lk(lock_);
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->assistant_,10000,assistant);
}

bool RobotCacheManager::GetAssistantInfo(const int64 platform_id,const int64& uid,robot_base::RobotBasicInfo& assistant){
	robot_logic::RLockGd lk(lock_);
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->assistant_,uid,assistant);
}

bool RobotCacheManager::SetAssistantLogin(const int64 platform_id,const int64 assistant_id,const int32 socket){
	bool r = false;
	robot_base::RobotBasicInfo assistant;
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	r = base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->assistant_,assistant_id,assistant);
	if(!r)
		return r;
	assistant.set_login_status(1);
	assistant.set_socket(socket);
	return base::MapAdd<RobotInfosMap,robot_base::RobotBasicInfo>(pc->assistant_,assistant_id,assistant);

}

bool RobotCacheManager::SendAssistantHandlseSong(const int64 platform_id,std::list<struct HandleSongInfo*>& list){
	robot_logic::RLockGd lk(lock_);
	bool r = false;
	robot_base::RobotBasicInfo assistant;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	r = base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->assistant_,10000,assistant);
	if(!r)
		return false;
	struct NoticeAssistantHandselSong notice_assistant_handselsong;
	MAKE_HEAD(notice_assistant_handselsong, NOTICE_ASSISTANT_HANDSEL_SONG,USER_TYPE,0,0);
	notice_assistant_handselsong.platform_id = platform_id;
	notice_assistant_handselsong.assistant_id = assistant.uid();
	notice_assistant_handselsong.list = list;
	return sendmessage(assistant.socket(),&notice_assistant_handselsong);
}

bool RobotCacheManager::SendAssistantLuckGift(const int64 platform_id,const int64 uid,
		const int32 share_plat,const int32 prize,const int64 songid){
	robot_logic::RLockGd lk(lock_);
	bool r = false;
	std::string music_str;
	std::string b64title;
	std::string b64artist;
	base::MusicInfo musicinfo;
	robot_base::RobotBasicInfo assistant;
	if(prize==0)
		return true;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	r = base::MapGet<RobotInfosMap,RobotInfosMap::iterator,robot_base::RobotBasicInfo>(pc->assistant_,10000,assistant);
	if(!r)
		return false;
	//获取歌曲信息
	r = robot_storage::RedisComm::GetMusicInfos(songid,music_str);
	if(r){
		musicinfo.UnserializedJson(music_str);
		Base64Decode(musicinfo.title(),&b64title);
		Base64Decode(musicinfo.artist(),&b64artist);
	}else{
		b64title = "未知";
		b64artist = "佚名";
	}

	//通过助手小哟聊天形式 通知用户中奖=
	struct NoticeUserRobotGiftLuck gift_luck;
	MAKE_HEAD(gift_luck, NOTICE_USER_ROBOT_GIFT_LUCK,USER_TYPE,0,0);
	gift_luck.platform_id = 10000;
	gift_luck.uid = uid;
	gift_luck.share_plat = share_plat;
	gift_luck.prize = prize;
	memset(&gift_luck.singer,'\0',SINGER_LEN);
	snprintf(gift_luck.singer, arraysize(gift_luck.singer),
						"%s",b64artist.c_str());
	memset(&gift_luck.name,'\0',NAME_LEN);
	snprintf(gift_luck.name, arraysize(gift_luck.name),
						"%s",b64title.c_str());
	return sendmessage(assistant.socket(),&gift_luck);
}



bool RobotCacheManager::NoticeAssistantLogin(const int64 platform_id){
	robot_logic::RLockGd lk(lock_);
	int i = 2;
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	robot_base::SchedulerInfo scheduler_info;
	r = GetIdleScheduler(pc->schduler_infos_,scheduler_info);
	if(!r)
		return false;
	RobotInfosMap::iterator it = pc->assistant_.begin();
	for(;it!=pc->assistant_.end();++it){
		robot_base::RobotBasicInfo robot = it->second;
		struct NoticeAssistantLogin notice_assistant_login;
		MAKE_HEAD(notice_assistant_login, NOTICE_ASSISTANT_LOGIN,USER_TYPE,0,0);
		notice_assistant_login.platform_id = platform_id;
		notice_assistant_login.assistant_id = robot.uid();
		memset(&notice_assistant_login.nickname,'\0',NICKNAME_LEN);
		snprintf(notice_assistant_login.nickname, arraysize(notice_assistant_login.nickname),
						"%s",robot.nickname().c_str());
		//通知调度
		//snprintf(notice_assistant_login.nickname, arraysize(notice_assistant_login.nickname),
		//		"%s",robot.nickname().c_str());
		sendmessage(scheduler_info.socket(),&notice_assistant_login);
		LOG_DEBUG2("scheduler socket %d",scheduler_info.socket());
	}
	return true;
}



bool RobotCacheManager::SetScheduler(const int64 platform_id,robot_base::SchedulerInfo& scheduler_info){
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapAdd<SchedulerMap,robot_base::SchedulerInfo>(pc->schduler_infos_,scheduler_info.socket(),scheduler_info);
}

bool RobotCacheManager::GetScheduler(const int64 platform_id,const int socket,robot_base::SchedulerInfo& scheduler_info){
	robot_logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapGet<SchedulerMap,SchedulerMap::iterator,robot_base::SchedulerInfo>(pc->schduler_infos_,socket,scheduler_info);
}

bool RobotCacheManager::DeleteScheduler(const int64 platform_id,const int socket){
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	return base::MapDel<SchedulerMap,SchedulerMap::iterator>(pc->schduler_infos_,socket);
}

bool RobotCacheManager::SchedulerSendMessage(const int64 platform_id,struct PacketHead* packet){
	robot_logic::WLockGd lk(lock_);
	robot_base::SchedulerInfo scheduler_info;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	bool r = GetIdleScheduler(pc->schduler_infos_,scheduler_info);
	if(!r)
		return false;

	LOG_DEBUG2("scheduler socket %d",scheduler_info.socket());
	return sendmessage(scheduler_info.socket(),packet);
}

bool RobotCacheManager::RobotHandselSong(const int64 platform_id){
	bool r = false;
	robot_logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	//遍历使用机器人MAP
	for(RobotInfosMap::iterator it = pc->used_robot_infos_.begin();
			it!=pc->used_robot_infos_.end();it++){
		robot_base::RobotBasicInfo robotinfo = it->second;
		//查找主体机器人
		for(std::map<int64,int64>::iterator  itr = robotinfo.follow_uid().begin();
				itr!=robotinfo.follow_uid().end();itr++){
			//获取用户信息
			robot_base::UserInfo userinfo;
			r = base::MapGet<UserInfoMap,UserInfoMap::iterator,robot_base::UserInfo>(pc->user_infos_,
					itr->first,userinfo);
			if(!r)
				continue;
			//检测是否赠送
			if(1){
				std::list<std::string> songinfolist;
				std::map<std::string,base::MusicInfo>  musicinfomap;
				std::list<int64> list;
				std::stringstream os;
				GetTypeRamdon(pc,userinfo.mode(),userinfo.type_id(),1,list);
				//获取音乐信息
				os<<userinfo.mode()<<"_r"<<userinfo.type_id();
				r = robot_storage::RedisComm::GetBatchMusicInfos(os.str(),list,songinfolist);
				if(songinfolist.size()<=0)
					continue;
				robot_logic::LogicUnit::FormateMusicInfo(songinfolist,musicinfomap);
				if(musicinfomap.size()<=0)
					continue;
				struct NoticeUserRobotHandselSong notice_handsel_song;
				MAKE_HEAD(notice_handsel_song, NOTICE_USER_ROBOT_HANDSEL_SONG,USER_TYPE,0,0);
				notice_handsel_song.platform_id = platform_id;
				notice_handsel_song.uid = itr->first;
				notice_handsel_song.robot_id = robotinfo.uid();
				notice_handsel_song.song_id = atoll(musicinfomap.begin()->second.id().c_str());
				sendrobotmssage(robotinfo,&notice_handsel_song);
				robotinfo.add_song_task_count();
			}
		}
	}
}

void RobotCacheManager::CheckRobotLive(const int64 platform_id){
	robot_logic::WLockGd lk(lock_);
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return;
	time_t current_time = time(NULL);
	RobotInfosMap::iterator it = pc->used_robot_infos_.begin();
	for(;it!=pc->used_robot_infos_.end();++it){
		robot_base::RobotBasicInfo robot_info = it->second;
		if(current_time - robot_info.follower_user_last_time()>3600){//超出限定时间，主动断掉机器人
			//断掉连接
			LOG_DEBUG("close connection");
			closelockconnect(robot_info.socket());
			continue;
			//通过响应事件将无响应模拟客户端移除，这里不移除
		}
	}
}

void RobotCacheManager::CheckSchedulerConnect(const int64 platform_id){
	robot_logic::WLockGd lk(lock_);
	bool r = false;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return;
	time_t current_time = time(NULL);
	SchedulerMap::iterator it = pc->schduler_infos_.begin();
	for(;it!=pc->schduler_infos_.end();it++){
		robot_base::SchedulerInfo scheduler = it->second;
		//先检测是否超过三次
		LOG_DEBUG2("robot_info.send_error_count %d",scheduler.send_error_count());
		if(scheduler.send_error_count()>3){
			//断掉连接
			LOG_DEBUG("close connection");
			closelockconnect(scheduler.socket());
			continue;
			//通过响应事件将无响应模拟客户端移除，这里不移除
		}
		if((current_time - scheduler.recv_last_time()>20)&&(current_time -scheduler.send_last_time()>20)){
			//发送心跳包
			struct PacketHead heart_packet;
			MAKE_HEAD(heart_packet, HEART_PACKET,USER_TYPE,0,0);
			sendschdulermessage(scheduler,&heart_packet);
			//r = false;
			if(!r)
				scheduler.add_send_error_count();
		}
	}

}

void RobotCacheManager::ChecAssistantConnect(const int64 platform_id){
	robot_logic::WLockGd lk(lock_);
	bool r = false;

	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return;
	time_t current_time = time(NULL);
	RobotInfosMap::iterator it = pc->assistant_.begin();
	for(;it!=pc->assistant_.end();it++){
		robot_base::RobotBasicInfo assistant_info = it->second;
		//先检测是否超过三次
		LOG_DEBUG2("robot_info.send_error_count %d",assistant_info.send_error_count());
		if(assistant_info.send_error_count()>3){
			//断掉连接
			LOG_DEBUG("close connection");
			closelockconnect(assistant_info.socket());
			continue;
			//通过响应事件将无响应模拟客户端移除，这里不移除
		}
		//检测当前接收时间是否大于20s//检测发送时间是否大于20s 若一个小于20s 皆不需要心跳包发送
		//LOG_DEBUG2("recv_last_time %lld send_last_time %lld current_time %lld recv_differ_time %lld send_differ_time %lld",robot_info.recv_last_time(),
		//		robot_info.send_last_time(),current_time,(current_time - robot_info.recv_last_time()),
		//		(current_time -robot_info.send_last_time()));
		if((current_time - assistant_info.recv_last_time()>20)&&(current_time -assistant_info.send_last_time()>20)){
			//发送心跳包
			struct PacketHead heart_packet;
			MAKE_HEAD(heart_packet, HEART_PACKET,USER_TYPE,0,0);
			sendrobotmssage(assistant_info,&heart_packet);
			//r = false;
			if(!r)
				assistant_info.add_send_error_count();
		}
	}
}
void RobotCacheManager::CheckRobotConnect(const int64 platform_id){
	robot_logic::WLockGd lk(lock_);
	bool r = false;

	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return;
	time_t current_time = time(NULL);
	RobotInfosMap::iterator it = pc->used_robot_infos_.begin();
	for(;it!=pc->used_robot_infos_.end();it++){
		robot_base::RobotBasicInfo robot_info = it->second;
		//先检测是否超过三次
		LOG_DEBUG2("robot_info.send_error_count %d",robot_info.send_error_count());
		if(robot_info.send_error_count()>3){
			//断掉连接
			LOG_DEBUG("close connection");
			closelockconnect(robot_info.socket());
			continue;
			//通过响应事件将无响应模拟客户端移除，这里不移除
		}
		//检测当前接收时间是否大于20s//检测发送时间是否大于20s 若一个小于20s 皆不需要心跳包发送
		//LOG_DEBUG2("recv_last_time %lld send_last_time %lld current_time %lld recv_differ_time %lld send_differ_time %lld",robot_info.recv_last_time(),
		//		robot_info.send_last_time(),current_time,(current_time - robot_info.recv_last_time()),
		//		(current_time -robot_info.send_last_time()));
		if((current_time - robot_info.recv_last_time()>20)&&(current_time -robot_info.send_last_time()>20)){
			//发送心跳包
			struct PacketHead heart_packet;
			MAKE_HEAD(heart_packet, HEART_PACKET,USER_TYPE,0,0);
			sendrobotmssage(robot_info,&heart_packet);
			//r = false;
			if(!r)
				robot_info.add_send_error_count();
		}
	}

}

void RobotCacheManager::Dump(){
	robot_logic::RLockGd lk(lock_);
	int64 platform_id =10000;
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return;
	LOG_DEBUG2("pc->used_robot_infos_.size() %lld,pc->idle_robot_infos_.size()) %lld",
			pc->used_robot_infos_.size(),pc->idle_robot_infos_.size());
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
		temp_robot.set_follower_user_last_time(time(NULL));
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


void RobotCacheManager::CreateTypeRamdon(PlatformCache* pc,std::string& type,
									  std::list<int64> &list){
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
        int num,std::list<int64>& list){
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


/****************CacheManagerOp*****************/
CacheManagerOp::CacheManagerOp() {
	InitThreadrw(&lock_);
	SetPrize();
}

CacheManagerOp::~CacheManagerOp() {
	DeinitThreadrw(lock_);
	if(prize_){
		delete prize_;
		prize_ = NULL;
	}
}


bool CacheManagerOp::SetPrize(){
	prize_ = new PrizeRate();
	return true;
}

int CacheManagerOp::GetPrize(pluck_prize prize_info,int count){
	robot_logic::RLockGd lk(lock_);
	return prize_->GetPrize(prize_info,count);
}

bool CacheManagerOp::SetRobotInfo(const int socket,const robot_base::RobotBasicInfo& robotinfo){
	robot_logic::WLockGd lk(lock_);
	return base::MapAdd<SocketRobotInfosMap,const int,const robot_base::RobotBasicInfo>(socket_robot_map_,socket,robotinfo);
}

bool CacheManagerOp::DelRobotInfo(const int socket){
	robot_logic::WLockGd lk(lock_);
	return base::MapDel<SocketRobotInfosMap,SocketRobotInfosMap::iterator,int>(socket_robot_map_,socket);
}

bool CacheManagerOp::GetRobotInfo(const int socket,robot_base::RobotBasicInfo& robotinfo){
	robot_logic::RLockGd lk(lock_);
	return base::MapGet<SocketRobotInfosMap,SocketRobotInfosMap::iterator,robot_base::RobotBasicInfo>(socket_robot_map_,socket,robotinfo);
}

bool CacheManagerOp::SetLuckGiftInfo(robot_base::LuckGiftInfo* luck){
	bool r = false;
	LuckGiftInfoPlatMap plat_map;
	robot_logic::WLockGd lk(lock_);
	//查找该平台是已经存在
	r = base::MapGet<LuckGiftInfoMap,LuckGiftInfoMap::iterator,LuckGiftInfoPlatMap>(luck_gift_infos_,luck->plat(),plat_map);

	//存在则添加
	plat_map[luck->prize()] = luck;
	//放入map
	return base::MapAdd<LuckGiftInfoMap,int,LuckGiftInfoPlatMap>(luck_gift_infos_,luck->plat(),plat_map);
}


bool CacheManagerOp::GetLuckGiftInfo(const int plat,LuckGiftInfoPlatMap& luck_gift_map_){
	bool r = false;
	robot_logic::RLockGd lk(lock_);
	return base::MapGet<LuckGiftInfoMap,LuckGiftInfoMap::iterator,LuckGiftInfoPlatMap>(luck_gift_infos_,plat,luck_gift_map_);

}

bool CacheManagerOp::FetchLuckGiftDB(){
	return robot_storage::DBComm::GetLuckGiftInfo(cache_manager_op_);
}

}
