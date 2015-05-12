#ifndef _ROBOT_ROBOT_ENGINE_H__
#define _ROBOT_ROBOT_ENGINE_H__
#include "music_mgr.h"
#include "user_mgr.h"
#include "plugins.h"
#include "protocol/http_packet.h"
#include "basic/scoped_ptr.h"
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <list>
namespace robot_logic{
class RobotEngine{
public:
	RobotEngine(void);
	~RobotEngine(void);
public:
	static RobotEngine* GetInstance(void);
	static void FreeInstance(void);
public:
	static RobotEngine*  engine_;
public:
public:
	bool OnRobotConnect(struct server* srv,int socket);

    bool OnRobotMessage(struct server *srv, int socket,
		                 const void *msg, int len);

	bool OnRobotClose(struct server *srv, int socket);

	bool OnBroadcastConnect(struct server *srv,
		                    int socket, void *data,
							int len);

	bool OnBroadcastMessage (struct server *srv,
		                     int socket, void *msg,
							 int len);

	bool OnBroadcastClose (struct server *srv, int socket);


	bool OnIniTimer (const struct server* srv);

	bool OnTimeout (struct server *srv, char* id, int opcode, int time);
public:
	bool Init();
private:
	scoped_ptr<robot_logic::MusicMgr>               music_mgr_;
	scoped_ptr<robot_logic::UserMgr>                user_mgr_;
	scoped_ptr<robot_logic::UtilMgr>                util_mgr_;

};

}

#endif
