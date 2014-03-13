#ifndef _CHAT_CHAT_CACHE_MANAGER_H__
#define _CHAT_CHAT_CACHE_MANAGER_H__

#include "logic_unit.h"
#include "base/thread_handler.h"
#include "base/thread_lock.h"

namespace chat_logic{

class PlatformCache{
public:
	chat_base::PlatformInfo   platform_info_;

	UserInfosMap              user_infos_map_;

	LeaveInfosMap             leave_infos_map_;

	MeetingMap                meeting_infos_map_;

};

class PlatformChatCacheManager{
public:
	PlatformChatCacheManager();
	virtual ~PlatformChatCacheManager();

	bool AddUserInfos(const int64 platform_id,const int64 user_id,
		const chat_base::UserInfo& userinfo);

	bool GetUserInfos(const int64 platform_id,const int64 user_id,
		chat_base::UserInfo& userinfo);

	bool DelUserInos(const int64 platform_id,const int64 user_id);

	bool AddLeaveInfos(const int64 platform_id,const int64 session,
						const int64 tid,const int64 mid);

	bool IsExitsLeaveInfos(const int64 platform_id,const int64 tid,
			               const int64 mid,const int64 session);

	bool DelLeaveInfos(const int64 platform_id,const int64 tid,const int64 mid);


	bool AddMeetingInfos(const int64 platform_id,const int64 session,
							const int64 tid,const int64 mid);

	bool DelMeetingInfos(const int64 platform_id,const int64 session,
							const int64 uid);

	bool SendQuitInfoSession(const int64 platform_id,const int64 session,const int32 uid);

private:
	std::map<int64,PlatformCache*>             platform_cache_;
	struct threadrw_t*                         lock_;
	PlatformCache *GetPlatformCache (int64 platform_id);

};


class CacheManagerOp{
private:
	static PlatformChatCacheManager  *platform_opertion_mgr_;

	CacheManagerOp() {}
	virtual ~CacheManagerOp() {}
public:
	static PlatformChatCacheManager* GetPlatformChatMgrCache (){
		if (platform_opertion_mgr_ == NULL)
			platform_opertion_mgr_ = new PlatformChatCacheManager ();

		return platform_opertion_mgr_;
	}
};

}
#endif
