#ifndef _CHAT_CHAT_MANAGER_H__
#define _CHAT_CHAT_MANAGER_H__

#include "user_connection.h"
#include "im_mgr.h"
#include "basic/scoped_ptr.h"
#include "plugins.h"
#include "common.h"

namespace chat_logic{

class ChatManager{
public:
	ChatManager();
	virtual ~ChatManager();

private:
	static ChatManager   *instance_;

public:
	static ChatManager *GetInstance();
	static void FreeInstance();

public:
	bool OnChatConnect (struct server *srv,const int socket);

	bool OnChatManagerMessage (struct server *srv, const int socket,
		                         const void *msg, const int len);

	bool OnChatManagerClose (struct server *srv,const int socket);

	bool OnBroadcastConnect (struct server *srv,const int socket,
		                     const void *data, const int len);

	bool OnBroadcastMessage (struct server *srv, const int socket,
		                     const void *msg, const int len);

	bool OnBroadcastClose (struct server *srv, const int socket);

	bool OnIniTimer (struct server *srv);

	bool OnTimeout (struct server *srv, char* id, int opcode, int time);
private:
	bool Init();
private:
	scoped_ptr<chat_logic::UserConnectionMgr>    usr_connection_mgr_;
	scoped_ptr<chat_logic::IMSMgr>               ims_mgr_;
};


}
#endif
