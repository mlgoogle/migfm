#ifndef _MASTER_PLUGIN_CHAT_MGR_SRV__H__
#define _MASTER_PLUGIN_CHAT_MGR_SRV__H__
#include "plugins.h"
#include "basic/http_packet.h"

namespace chat_logic{

class ServerMgr{
public:
	ServerMgr();
	virtual ~ServerMgr();
public:
	bool GetBestIdle(const int socket,const packet::HttpPacket& packet);
	bool GetBestIdleTest(const int socket,const packet::HttpPacket& packet);

};
}

#endif
