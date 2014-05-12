#ifndef _MASTER_PLUGIN_CHAT_MGR_CHAT_MGR_ENGINE_H__
#define _MASTER_PLUGIN_CHAT_MGR_CHAT_MGR_ENGINE_H__
#include "plugins.h"
#include "message_mgr.h"
#include "srv_mgr.h"
#include "protocol/http_packet.h"
#include "basic/scoped_ptr.h"

namespace chat_logic{

class ChatMgrEngine{
public:
	ChatMgrEngine();
	virtual ~ChatMgrEngine();

	static ChatMgrEngine *GetInstance();
	static void FreeInstance();

private:
	static ChatMgrEngine   *instance_;

public:
	bool OnChatMgrConnect(struct server* srv,int socket);

    bool OnChatMgrMessage(struct server *srv, int socket,
		                 const void *msg, int len);

	bool OnChatMgrClose(struct server *srv, int socket);

	bool OnBroadcastConnect(struct server *srv, 
		                    int socket, void *data, 
							int len);

	bool OnBroadcastMessage (struct server *srv, 
		                     int socket, void *msg, 
							 int len);

	bool OnBroadcastClose (struct server *srv, int socket);


	bool OnIniTimer (const struct server* srv);

	bool OnTimeout (struct server *srv, char* id, int opcode, int time);
private:
private:
	scoped_ptr<chat_logic::MessageMgr>    			message_mgr_;
	scoped_ptr<chat_logic::ServerMgr>               srv_mgr_;

};

}
#endif
