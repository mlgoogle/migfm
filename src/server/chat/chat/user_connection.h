#ifndef _CHAT_USER_CONNECTION_H__
#define _CHAT_USER_CONNECTION_H__

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
private:
	bool OnGetUserInfo(const int socket,const int64 platform_id,
			           const int64 user_id,const int64 oppinfo_id,const int32 type,
			           const int64 usr_session);

	bool ClearUserinfo(const int64 platform_id,const int64 user_id);
};

}
#endif
