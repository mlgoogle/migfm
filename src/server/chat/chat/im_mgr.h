#ifndef _CHAT_IM_MGR_H__
#define _CHAT_IM_MGR_H__

#include "chat_basic_infos.h"
#include "common.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include <string>

namespace chat_logic{


class IMSMgr{
public:
	IMSMgr();
	virtual ~IMSMgr();

public:
	bool OnMessage(struct server *srv, int socket, struct PacketHead *packet,
		           const void *msg = NULL, int len = 0);

	bool OnCreatePrviateChat(struct server *srv,int socket,struct PacketHead *packet,
					const void *msg = NULL, int len = 0);

	bool OnConfirmMessage(struct server *srv,int socket,struct PacketHead *packet,
					const void *msg = NULL, int len = 0);

private:
	bool LeaveMessage(const int64 platform_id,const int64 msg_id,const time_t current_time,
			const chat_base::UserInfo& send_userinfo,
			const chat_base::UserInfo& recv_userinfo,const std::string& message);

	bool PushMessage(const int64 platform_id,const chat_base::UserInfo& send_userinfo,
			const chat_base::UserInfo& recv_userinfo,const std::string& message);

};

}

#endif
