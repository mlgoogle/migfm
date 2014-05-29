#ifndef _MASTER_PLUGIN_SSO_IDENTITY_PROVIDER_ENGINE_H__
#define _MASTER_PLUGIN_SSO_IDENTITY_PROVIDER_ENGINE_H__
#include "plugins.h"
#include "basic/http_packet.h"
namespace sso_logic{

class IdentityProviderEngine{
public:
	IdentityProviderEngine();
	virtual ~IdentityProviderEngine();

	static IdentityProviderEngine *GetInstance();
	static void FreeInstance();

private:
	static IdentityProviderEngine   *instance_;

public:
	bool OnConnect(struct server* srv,int socket);

    bool OnMessage(struct server *srv, int socket, 
		                 const void *msg, int len);

	bool OnClose(struct server *srv, int socket);

	bool OnBroadcastConnect(struct server *srv, 
		                       int socket, void *data, 
							   int len);

	bool OnBroadcastMessage (struct server *srv, 
		                     int socket, void *msg, 
							 int len);

	bool OnBroadcastClose (struct server *srv, int socket);


	bool OnIniTimer (const struct server* srv);

	bool OnTimeout (struct server *srv, char* id, int opcode, int time);
};

}
#endif