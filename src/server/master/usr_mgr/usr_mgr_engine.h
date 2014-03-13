#ifndef _MASTER_PLUGIN_USR_MGR_USR_MGR_ENGINE_H__
#define _MASTER_PLUGIN_USR_MGR_USR_MGR_ENGINE_H__
#include "plugins.h"
#include "basic/http_packet.h"
namespace chat_logic{

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

	bool RegistUser(const int socket,const packet::HttpPacket& packet);

	bool UpdateUserinfo(const int socket,const packet::HttpPacket& packet);

	bool GetUserInfo(const int socket,const packet::HttpPacket& packet);

	bool CreateGuest(const int socket,const packet::HttpPacket& packet);

	bool UserLogin(const int socket,const packet::HttpPacket& packet);

	void GetResultMsg(std::string &status, std::string &msg,
		              std::string &result,std::string &out_str);
private:
	bool RegeditUsr(const int socket,const int flag,const std::string username,
		            const std::string password,const std::string nickname,
	                const std::string source,const std::string gender = "1",
	                const std::string type = "1",const std::string birthday = "1986-10-01",
	                const std::string location = "浙江省杭州市",
					const std::string head = "http://fm.miglab.com/default.jpg");

	bool CreateGuest(const int socket,const int flag,const std::string username,
		             const std::string password,const std::string nickname,
		             const std::string source,const std::string gender = "1",
		             const std::string type = "1",const std::string birthday = "1986-10-01",
		             const std::string location = "浙江省杭州市",
		             const std::string head = "http://fm.miglab.com/default.jpg");
};

}
#endif
