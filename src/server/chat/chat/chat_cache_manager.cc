#include "chat_cache_manager.h"
#include "chat_basic_infos.h"
#include "db_comm.h"
#include "intertface/robot_interface.h"
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

bool PlatformChatCacheManager::AddConfirmMessage(const int64 platform_id,const int64 msg_id,
													chat_base::UserInfo& user_info){
	logic::WLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if(pl==NULL)
		return false;
	return base::MapAdd<ConfirmMap,chat_base::UserInfo>(pl->confirm_message_map_,msg_id,user_info);
}

bool PlatformChatCacheManager::DelConfirmMessage(const int64 platform_id,const int64 msg_id){
	logic::WLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if(pl==NULL)
		return false;
	return base::MapDel<ConfirmMap,ConfirmMap::iterator>(pl->confirm_message_map_,msg_id);
}

bool PlatformChatCacheManager::AddGroupInfos(const int64 platform_id,const int64 group_id,
		const chat_base::GroupInfo& groupinfo){
	logic::WLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
		return false;
	return base::MapAdd<GroupInfosMap,chat_base::GroupInfo>(pl->group_infos_map_,group_id,groupinfo);
}

bool PlatformChatCacheManager::GetGroupInfos(const int64 platform_id,const int64 group_id,
						chat_base::GroupInfo& groupinfo){
	logic::RLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
		return false;
	return base::MapGet<GroupInfosMap,GroupInfosMap::iterator,chat_base::GroupInfo>(pl->group_infos_map_,group_id,groupinfo);
}

bool PlatformChatCacheManager::DelGroupInfos(const int64 platform_id,const int64 group_id){
	logic::WLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
		return false;
	return base::MapDel<GroupInfosMap,GroupInfosMap::iterator>(pl->group_infos_map_,group_id);
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

bool PlatformChatCacheManager::CheckMeetingInfos(const int64 platform_id,const int64 session,
		const int64 uid){
	logic::RLockGd lk(lock_);
	bool r = false;
	int64 temp_uid;
	MeetingSession meeting_session;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if(pl==NULL)
		return false;
	r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);
	if(r)
		return base::MapGet<MeetingSession,MeetingSession::iterator,int64>(meeting_session,uid,temp_uid);
	return false;
}

bool PlatformChatCacheManager::AddMeetingInfos(const int64 platform_id,const int64 session,
							const int64 uid){
	logic::WLockGd lk(lock_);
	bool r = false;
	MeetingSession meeting_session;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if(pl==NULL)
		return false;


	r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);
	//
	if(r){
		meeting_session[uid] = uid;
		return base::MapAdd(pl->meeting_infos_map_,session,meeting_session);
	}else{
		meeting_session[uid] = uid;
		return base::MapAdd(pl->meeting_infos_map_,session,meeting_session);
	}
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

	r = base::MapGet<MeetingMap,MeetingMap::iterator,int64,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);
	if(meeting_session.size()>2){//群聊
		base::MapDel<MeetingSession,MeetingSession::iterator>(meeting_session,uid);
	}else if(meeting_session.size()==2){
		base::MapDel<MeetingMap,MeetingMap::iterator>(pl->meeting_infos_map_,session);
		//获取 双方uid
		int64 tid = 0;
		for(MeetingSession::iterator it = meeting_session.begin();it!=meeting_session.end();it++){
			if(it->first!=uid)
				tid = it->first;
		}
		//加入到离线会话
		SessionInfosMap session_infos;
		session_infos[tid] = session;
		r = base::MapAdd<LeaveInfosMap,SessionInfosMap>(pl->leave_infos_map_,uid,session_infos);
	}else{//
		r = base::MapDel<LeaveInfosMap,LeaveInfosMap::iterator>(pl->leave_infos_map_,uid);
	}
	//DelMeetings
	/*
	r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);
	if(!r)
		base::MapDel<MeetingSession,MeetingSession::iterator>(meeting_session,uid);
	if(meeting_session.size()>0){
		return base::MapAdd(pl->meeting_infos_map_,session,meeting_session);
	}else{
		return base::MapDel<MeetingMap,MeetingMap::iterator>(pl->meeting_infos_map_,session);
	}*/
}

bool PlatformChatCacheManager::SendMeetingNotSelf(const int64 platform_id,const int64 group_id,
							const int64 send_id,const int64 session,struct PacketHead* packet){
	logic::RLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
		return false;
	MeetingSession meeting_session;
	bool r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);

	for(MeetingSession::iterator it = meeting_session.begin();it!=meeting_session.end();++it){
		chat_base::UserInfo userinfo;
		int id = it->first;
		r = base::MapGet<UserInfosMap,UserInfosMap::iterator,chat_base::UserInfo>(pl->user_infos_map_,id,userinfo);
		if(r&&userinfo.user_id()!=send_id){
			logic::SomeUtils::SendMessage(userinfo.socket(),packet,__FILE__,__LINE__);
		}
	}
	return true;
}
bool PlatformChatCacheManager::SendMeetingMessage(const int64 platform_id,const int64 group_id,
		const int64 session,struct PacketHead *packet){
	logic::RLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
		return false;
	MeetingSession meeting_session;
	bool r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);

	for(MeetingSession::iterator it = meeting_session.begin();it!=meeting_session.end();++it){
		chat_base::UserInfo userinfo;
		int id = it->first;
		r = base::MapGet<UserInfosMap,UserInfosMap::iterator,chat_base::UserInfo>(pl->user_infos_map_,id,userinfo);
		if(r){
			logic::SomeUtils::SendMessage(userinfo.socket(),packet,__FILE__,__LINE__);
		}
	}
	return true;
}

bool PlatformChatCacheManager::SendQuitInfoSession(const int64 platform_id,const int64 session,
		const int32 uid){
	bool r = false;
	struct UserQuit user_quit;
	MAKE_HEAD(user_quit, USER_NOTIFICATION_QUIT,USER_TYPE,0,0);
	user_quit.platform_id = platform_id;
	user_quit.user_id = uid;
	return SendMeetingNotSelf(platform_id,session,uid,session,&user_quit);
	/*MeetingSession meeting_session;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if(pl==NULL)
		return false;
	r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,session,meeting_session);

	for(MeetingSession::iterator it = meeting_session.begin();it!=meeting_session.end();++it){
		chat_base::UserInfo userinfo;
		int id = it->first;
		r = base::MapGet<UserInfosMap,UserInfosMap::iterator,chat_base::UserInfo>(pl->user_infos_map_,id,userinfo);
		if(r){
			struct UserQuit user_quit;
			MAKE_HEAD(user_quit, USER_NOTIFICATION_QUIT,USER_TYPE,0,0);
			user_quit.platform_id = platform_id;
			user_quit.user_id = uid;
			//sendmessage(userinfo.socket(),&user_quit);
			logic::SomeUtils::SendMessage(userinfo.socket(),&user_quit,__FILE__,__LINE__);
		}
	}*/

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

	//
	SessionInfosMap session_infos;
	r = base::MapGet<LeaveInfosMap,LeaveInfosMap::iterator,SessionInfosMap>(pl->leave_infos_map_,mid,session_infos);

	if(!r)
		return false;

	r = base::MapGet<SessionInfosMap,SessionInfosMap::iterator,int64>(session_infos,tid,session);
	if(!r)
		return false;
	return true;
}

bool PlatformChatCacheManager::ClearLeaveInfos(const int64 platform_id,const int64 tid){
	logic::WLockGd lk(lock_);
	bool r = false;
	SessionInfosMap session_infos;
	PlatformCache* pl = GetPlatformCache(platform_id);
	if (pl==NULL)
	   return false;
	r = base::MapDel<LeaveInfosMap,LeaveInfosMap::iterator>(pl->leave_infos_map_,tid);
	return r;
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

bool PlatformChatCacheManager::GetGroupListUserInfoNotSelf(const int64 platform_id,const int64 group_id,const int64 send_id,
		const int64 session,std::list<struct Oppinfo*>& oppoinfo_list){
	logic::RLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	MeetingSession meeting_session;
	if (pl==NULL)
	   return false;
	bool r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,
			session,meeting_session);
	if(!r)
		return false;
	for(MeetingSession::iterator it = meeting_session.begin();
			it!=meeting_session.end();++it){
		int64 uid = it->second;
		chat_base::UserInfo userinfo;
		if(uid==send_id)
			continue;
		r = base::MapGet<UserInfosMap,UserInfosMap::iterator,chat_base::UserInfo>(pl->user_infos_map_,uid,
				userinfo);
		if(r){
			struct Oppinfo* oppinfo = new struct Oppinfo;
		    oppinfo->user_id = userinfo.user_id();
		    oppinfo->user_nicknumber = userinfo.nicknumber();
			logic::SomeUtils::SafeStrncpy(oppinfo->nickname,NICKNAME_LEN,
					userinfo.nickname().c_str(),userinfo.nickname().length());

		    logic::SomeUtils::SafeStrncpy(oppinfo->user_head,HEAD_URL_LEN,
		    		userinfo.head_url().c_str(),userinfo.head_url().length());

		    oppoinfo_list.push_back(oppinfo);
		}
	}
	return true;
}

bool PlatformChatCacheManager::GetGroupListUserInfo(const int64 platform_id,const int64 group_id,const int64 session,
			std::list<struct Oppinfo*>& oppoinfo_list){
	logic::RLockGd lk(lock_);
	PlatformCache* pl = GetPlatformCache(platform_id);
	MeetingSession meeting_session;
	if (pl==NULL)
	   return false;
	bool r = base::MapGet<MeetingMap,MeetingMap::iterator,MeetingSession>(pl->meeting_infos_map_,
			session,meeting_session);
	if(!r)
		return false;
	for(MeetingSession::iterator it = meeting_session.begin();
			it!=meeting_session.end();++it){
		int64 uid = it->second;
		chat_base::UserInfo userinfo;
		r = base::MapGet<UserInfosMap,UserInfosMap::iterator,chat_base::UserInfo>(pl->user_infos_map_,uid,
				userinfo);
		if(r){
			struct Oppinfo* oppinfo = new struct Oppinfo;
		    oppinfo->user_id = userinfo.user_id();
		    oppinfo->user_nicknumber = userinfo.nicknumber();
			logic::SomeUtils::SafeStrncpy(oppinfo->nickname,NICKNAME_LEN,
					userinfo.nickname().c_str(),userinfo.nickname().length());

		    logic::SomeUtils::SafeStrncpy(oppinfo->user_head,HEAD_URL_LEN,
		    		userinfo.head_url().c_str(),userinfo.head_url().length());

		    oppoinfo_list.push_back(oppinfo);
		}
	}
	return true;
}

CacheManagerOp::CacheManagerOp()
:robot_server_socket_(0){
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

void CacheManagerOp::SetRobotServerSocket(const int socket){
	logic::RLockGd lk(lock_);
	robot_server_socket_ = socket;
}

bool CacheManagerOp::NoticeRobotChatLogin(const int64 platform_id,const int64 uid,
		const int64 robotid){
	NoticeRobotLogin(robot_server_socket_,platform_id,uid,robotid);
	return true;
}

bool CacheManagerOp::FetchDBDimension(){
	chat_storage::DBComm::GetDimensionGroup(platform_opertion_mgr_);
}

}
