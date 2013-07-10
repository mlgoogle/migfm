#ifndef _MASTER_PLUGIN_USR_MGR_USR_MGR_ENGINE_H__
#define _MASTER_PLUGIN_USR_MGR_USR_MGR_ENGINE_H__
#include "plugins.h"
#include "basic/http_packet.h"
namespace usr_logic{

class UsrMgrEngine{
public:
	UsrMgrEngine();
	virtual ~UsrMgrEngine();

	static UsrMgrEngine *GetInstance();
	static void FreeInstance();

private:
	static UsrMgrEngine   *instance_;

public:
	bool OnUsrMgrConnect(struct server* srv,int socket);

    bool OnUsrMgrMessage(struct server *srv, int socket, 
		                 const void *msg, int len);

	bool OnUsrMgrClose(struct server *srv, int socket);

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
	bool RegeditUsr(const int socket,const packet::HttpPacket& packet);

	bool UpdateUserinfo(const int socket,const packet::HttpPacket& packet);

	bool GetUserInfo(const int socket,const packet::HttpPacket& packet);
	
	void GetResultMsg(std::string &status, std::string &msg,
		              std::string &result,std::string &out_str);
};

}
#endif