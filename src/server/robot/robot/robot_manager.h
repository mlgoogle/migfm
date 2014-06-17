#ifndef _ROBOT_ROBOT_MANAGER_H__
#define _ROBOT_ROBOT_MANAGER_H__

#include "scheduler_mgr.h"
#include "robot_connection.h"
#include "robot_connection.h"
#include "basic/scoped_ptr.h"
#include "plugins.h"
#include "common.h"

namespace robot_logic{

class RobotManager{
public:
	RobotManager();
	virtual ~RobotManager();

private:
	static RobotManager   *instance_;

public:
	static RobotManager *GetInstance();
	static void FreeInstance();

public:
	bool OnRobotConnect (struct server *srv,const int socket);

	bool OnRobotManagerMessage (struct server *srv, const int socket,
		                         const void *msg, const int len);

	bool OnRobotManagerClose (struct server *srv,const int socket);

	bool OnBroadcastConnect (struct server *srv,const int socket,
		                     const void *data, const int len);

	bool OnBroadcastMessage (struct server *srv, const int socket,
		                     const void *msg, const int len);

	bool OnBroadcastClose (struct server *srv, const int socket);

	bool OnIniTimer (struct server *srv);

	bool OnTimeout (struct server *srv, char* id, int opcode, int time);
private:
	bool Init();
	bool InitDefaultPlatformInfo();
private:
	scoped_ptr<robot_logic::SchedulerMgr>    scheduler_mgr_;
	scoped_ptr<robot_logic::RobotConnection>  robot_mgr_;
};


}
#endif
