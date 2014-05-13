#ifndef _MASTER_PLUGIN_MANAGER_MANAGER_ENGINE_H__
#define _MASTER_PLUGIN_MANAGER_MANAGER_ENGINE_H__
#include "plugins.h"
#include "protocol/http_packet.h"
#include "basic/scoped_ptr.h"

namespace manager_logic{

class ManagerEngine{
public:
	ManagerEngine();
	virtual ~ManagerEngine();

	static ManagerEngine *GetInstance();
	static void FreeInstance();

private:
	static ManagerEngine   *instance_;

public:
	bool OnManagerConnect(struct server* srv,int socket);

    bool OnManagerMessage(struct server *srv, int socket,
		                 const void *msg, int len);

	bool OnManagerClose(struct server *srv, int socket);

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
	bool PushUserMsg(const int socket,const packet::HttpPacket& packet);

	bool PushAllUserMsg(const int socket,const packet::HttpPacket& packet);

private:
	void PushUserMessage(const int64 recvid,const std::string& summary,
				const std::string& message,const int32 is_record,
				const int32 is_push);

	bool PushMessage(const int64 platform_id,const int64 recvid,
			const std::string& message);

	bool OffLineMessage(const int64 platform_id,const int64 msg_id,
            const time_t current_time,const int64&  recv_uid,
            const std::string& message);

	bool LeaveMessage(const int64 platform_id,const int64 msg_id,const time_t current_time,
			const int64& recvid,const std::string& message);

};

}
#endif
