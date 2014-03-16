#include "chat_cache_manager.h"
#include "chat_basic_infos.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "basic/template.h"

namespace chat_logic{


PlatformChatCacheManager* CacheManagerOp::platform_opertion_mgr_ = NULL;
CacheManagerOp* CacheManagerOp::cache_manager_op_ = NULL;

PlatformChatCacheManager::PlatformChatCacheManager(){
    InitThreadrw(&lock_);
}

PlatformChatCacheManager::~PlatformChatCacheManager(){
    DeinitThreadrw(lock_);
}

void PlatformChatCacheManager::SetPlatformInfo(const int64 platform_id,chat_base::PlatformInfo& platform){
	logic::WLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc == NULL){
		pc = new PlatformCache;
		platform_cache_[platform_id] = pc;
	}
	pc->platform_info_ = platform;
}

bool PlatformChatCacheManager::GetPlatformInfo(const int64 platform_id,chat_base::PlatformInfo& platform){
	logic::RLockGd lk(lock_);
	PlatformCache* pc = GetPlatformCache(platform_id);
	if(pc==NULL)
		return false;
	platform = pc->platform_info_;
	return true;
}

PlatformCache* PlatformChatCacheManager::GetPlatformCache(int64 platform_id){
	std::map<int64,PlatformCache*>::iterator it
		= platform_cache_.find(platform_id);

	if(it == platform_cache_.end())
		return NULL;
	return it->second;
}


bool PlatformChatCacheManager::AddUserInfos(const int64 platform_id,
											const int64 user_id, 
											const chat_base::UserInfo& userinfo){
	logic::WLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
		return false;
	return base::MapAdd<UserInfosMap,chat_base::UserInfo >(pl->user_infos_map_,
								user_id,userinfo);
}

bool PlatformChatCacheManager::GetUserInfos(const int64 platform_id,
											const int64 user_id, 
											chat_base::UserInfo& userinfo){
	logic::RLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
		return false;
	return base::MapGet<UserInfosMap,UserInfosMap::iterator,chat_base::UserInfo>(pl->user_infos_map_,user_id,userinfo);

}

bool PlatformChatCacheManager::DelUserInos(const int64 platform_id,
										   const int64 user_id){
	logic::RLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
	   return false;
	return base::MapDel<UserInfosMap,UserInfosMap::iterator>(pl->user_infos_map_,user_id);
}


bool PlatformChatCacheManager::AddMeetingInfos(const int64 platform_id,const int64 session,
							const int64 tid,const int64 mid){
	logic::WLockGd lk(lock_);
	bool r = false;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
	   return false;

	MeetingSession meeting;
	meeting[tid] = tid;
	meeting[mid] = mid;

	return base::MapAdd<MeetingMap,MeetingSession>(pl->meeting_infos_map_,session,meeting);
}

bool PlatformChatCacheManager::DelMeetingInfos(const int64 platform_id,const int64 session,const int64 uid){
	logic::WLockGd lk(lock_);
	bool r = false;
	MeetingSession meeting_session;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if(pl==NULL)
		return false;

	//DelMeetings
	r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);
	if(!r)
		base::MapDel<MeetingSession,MeetingSession::iterator>(meeting_session,uid);
	if(meeting_session.size()>0)
		return base::MapAdd(pl->meeting_infos_map_,session,meeting_session);
	else
		return base::MapDel<MeetingMap,MeetingMap::iterator>(pl->meeting_infos_map_,session);
}

bool PlatformChatCacheManager::SendQuitInfoSession(const int64 platform_id,const int64 session,const int32 uid){
	logic::RLockGd lk(lock_);
	bool r = false;
	MeetingSession meeting_session;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if(pl==NULL)
		return false;
	r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);

	for(MeetingSession::iterator it = meeting_session.begin();it!=meeting_session.end();++it){
		chat_base::UserInfo userinfo;
		int id = it->first;
		r = base::MapGet<UserInfosMap,UserInfosMap::iterator,chat_base::UserInfo>(pl->user_infos_map_,id,userinfo);
		if(!r){
			struct UserQuit user_quit;
			MAKE_HEAD(user_quit, USER_QUIT,USER_TYPE,0,0);
			user_quit.platform_id = platform_id;
			user_quit.user_id = uid;
			//sendmessage(userinfo.socket(),&user_quit);
			logic::SomeUtils::SendMessage(userinfo.socket(),&user_quit,__FILE__,__LINE__);
		}
	}

}

bool PlatformChatCacheManager::AddLeaveInfos(const int64 platform_id,const int64 session,
						const int64 tid,const int64 mid){

	logic::WLockGd lk(lock_);
	bool r = false;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
	   return false;

	//
	SessionInfosMap session_infos;
	r = base::MapGet<LeaveInfosMap,LeaveInfosMap::iterator,SessionInfosMap>(pl->leave_infos_map_,tid,session_infos);

	session_infos[mid] = session;

	r = base::MapAdd<LeaveInfosMap,SessionInfosMap>(pl->leave_infos_map_,tid,session_infos);
	return r;
}


bool PlatformChatCacheManager::IsExitsLeaveInfos(const int64 platform_id,const int64 tid,
			               const int64 mid,int64& session){
	logic::RLockGd lk(lock_);
	bool r = false;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
	   return false;

	//查询自己是否已经和对方产生会话
	SessionInfosMap session_infos;
	r = base::MapGet<LeaveInfosMap,LeaveInfosMap::iterator,SessionInfosMap>(pl->leave_infos_map_,mid,session_infos);

	if(!r)
		return false;

	r = base::MapGet<SessionInfosMap,SessionInfosMap::iterator,int64>(session_infos,tid,session);
	if(!r)
		return false;
	return true;
}

bool PlatformChatCacheManager::DelLeaveInfos(const int64 platform_id,const int64 tid,const int64 mid){
	logic::WLockGd lk(lock_);
	bool r = false;
	SessionInfosMap session_infos;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
	   return false;
	r = base::MapGet<LeaveInfosMap,LeaveInfosMap::iterator,SessionInfosMap>(pl->leave_infos_map_,tid,session_infos);
	if(!r)//no exits
		return false;
	r = base::MapDel<SessionInfosMap,SessionInfosMap::iterator>(session_infos,mid);

	// if sessioninfos = 0 clear leave_infos_map
	if(session_infos.size()>0){
		r = base::MapAdd<LeaveInfosMap,SessionInfosMap>(pl->leave_infos_map_,tid,session_infos);
	}else{
		r = base::MapDel<LeaveInfosMap,LeaveInfosMap::iterator>(pl->leave_infos_map_,tid);
	}
	return r;
}


CacheManagerOp::CacheManagerOp(){
	InitThreadrw(&lock_);
}

CacheManagerOp::~CacheManagerOp(){
	DeinitThreadrw(lock_);
}


bool CacheManagerOp::AddSocket(const int socket,const chat_base::UserInfo& userinfo){
	logic::WLockGd lk(lock_);
	bool r = false;

	r = base::MapAdd<SocketMap,chat_base::UserInfo>(socket_infos_map_,socket,userinfo);
	return r;
}

bool CacheManagerOp::GetSocket(const int socket,chat_base::UserInfo& userinfo){
	logic::RLockGd lk(lock_);
	bool r = false;

	r = base::MapGet<SocketMap,SocketMap::iterator,chat_base::UserInfo>(socket_infos_map_,socket,userinfo);
	return r;
}

bool CacheManagerOp::DelSocket(const int socket){
	logic::WLockGd lk(lock_);
	bool r = base::MapDel<SocketMap,SocketMap::iterator>(socket_infos_map_,socket);
	return r;
}

}
