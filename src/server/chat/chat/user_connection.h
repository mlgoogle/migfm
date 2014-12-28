#ifndef _CHAT_USER_CONNECTION_H__
#define _CHAT_USER_CONNECTION_H__

#include "chat_basic_infos.h"
#include "logic_unit.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include "common.h"

namespace chat_logic{

class UserConnectionMgr{
public:
	UserConnectionMgr();
	virtual ~UserConnectionMgr();
public:
	bool OnUserLogin(struct server *srv, int socket, struct PacketHead *packet,
		        const void *msg = NULL, int len = 0);

	bool OnGetOppInfos(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);

	bool OnUserQuit(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);

	bool OnUserGroupOnline(struct server *srv, int socket, struct PacketHead *packet,
	           const void *msg = NULL, int len = 0);
public:
	bool OnAberrant(const int socket);
private:
	bool OnGetUserInfo(const int socket,const int64 platform_id,
			           const int64 user_id,const int64 oppinfo_id,const int32 type,
			           const int64 usr_session);

	bool OnGetTempGroupInfo(const int socket,const int64 platform_id,
			const int64 user_id,const int64 oppinfo_id,const int32 type,
			const int64 usr_session);

	bool OnGetGroupInfo(const int socket,const int64 platform_id,
			const int64 user_id,const int64 oppinfo_id,const int32 type,
			const int64 usr_session);

	bool OnCreateGroupInfo(chat_base::GroupInfo& group_info,const int64 platform_id,const int64 group_id,const int16 type,
			const int64 session,const std::string& name = default_group_name,
			const std::string& head_url=default_group_url);


	bool ClearUserinfo(const int64 platform_id,const int64 user_id,const int64 session);
};

}
#endif
