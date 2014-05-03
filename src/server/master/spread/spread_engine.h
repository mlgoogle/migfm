#ifndef _MASTER_PLUGIN_MANAGER_SPREAD_ENGINE_H__
#define _MASTER_PLUGIN_MANAGER_SPREAD_ENGINE_H__
#include "weixin_mgr.h"
#include "plugins.h"
#include "basic/http_packet.h"
#include "basic/scoped_ptr.h"

namespace spread_logic{

class SpreadEngine{
public:
	SpreadEngine();
	virtual ~SpreadEngine();

	static SpreadEngine *GetInstance();
	static void FreeInstance();

private:
	static SpreadEngine   *instance_;

public:
	bool OnSpreadConnect(struct server* srv,int socket);

    bool OnSpreadMessage(struct server *srv, int socket,
		                 const void *msg, int len);

	bool OnSpreadClose(struct server *srv, int socket);

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
	scoped_ptr<spread_logic::WeiXinMgr>    			weixin_mgr_;
};

}
#endif
