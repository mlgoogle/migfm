#include "robot_init.h"
#include "robot_manager.h"
#include "logic_unit.h"
#include "logic/logic_comm.h"
#include "common.h"
#include "plugins.h"


struct RobotManagerPlugin{
	char* id;
	char* name;
	char* version;
	char* provider;
};

static void *OnRobotManagerStart(){
    LOG_DEBUG("OnRobotManagerStart");
	signal(SIGPIPE,SIG_IGN);
	struct RobotManagerPlugin* robot_mgr
		= (struct RobotManagerPlugin*) calloc(1,sizeof(struct RobotManagerPlugin));
	robot_mgr->id = PLUGIN_ID;
	robot_mgr->name = PLUGIN_NAME;
	robot_mgr->version = PLUGIN_VERSION;
	robot_mgr->provider = PLUGIN_PROVIDER;

	if (!robot_logic::RobotManager::GetInstance())
		assert(0);

	LOG_MSG("RobotManagerPlugin was started");
	return robot_mgr;
}

static handler_t OnRobotManagerShutdown(struct server* srv,void* pd){
	robot_logic::RobotManager::FreeInstance();
	LOG_MSG("OnRobotManagerShutdown was shutdown");
	return HANDLER_GO_ON;
}

static handler_t OnRobotManagerConnect(struct server *srv, int fd,
										void *data, int len){

	LOG_DEBUG("OnRobotManagerConnect");
	robot_logic::RobotManager::GetInstance()->OnRobotConnect(srv,fd);
	return HANDLER_GO_ON;
}

static handler_t OnRobotCenterConectClose(struct server* srv,int fd){
	
	LOG_DEBUG("OnRobotCenterConectClose");
	robot_logic::RobotManager::GetInstance()->OnRobotManagerClose(srv,fd);
	return HANDLER_GO_ON;
}

static handler_t OnRobotCenterMessage(struct server *srv, int sock,
										void *data, int len){

	LOG_DEBUG("OnRobotCenterMessage");
	robot_logic::RobotManager::GetInstance()->OnRobotManagerMessage(srv,sock,data,len);
	return HANDLER_GO_ON;
}

static handler_t OnUnknow (struct server *srv, int fd, void *data)
{
	LOG_DEBUG ("OnUnknow");
	return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect (struct server *srv, int fd, 
									 void *data, int len)
{
	LOG_DEBUG ("OnBroadcastConnect");
	robot_logic::RobotManager::GetInstance()->OnBroadcastConnect(srv,fd,data,len);
	return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose (struct server *srv, int fd)
{
	LOG_DEBUG ("OnBroadcastClose");
	robot_logic::RobotManager::GetInstance()->OnBroadcastClose(srv,fd);
	return HANDLER_GO_ON;

}
static handler_t OnBroadcastMessage (struct server *srv, int fd, void *data, int len)
{
	LOG_DEBUG ("OnBroadcastMessage");
	robot_logic::RobotManager::GetInstance()->OnBroadcastMessage(srv,fd,data,len);
	return HANDLER_GO_ON;
}

static handler_t OnClock (struct server *srv, int fd)
{
	LOG_DEBUG ("OnClock");
	return HANDLER_GO_ON;
}

static handler_t OnIniTimer (struct server *srv){
	LOG_DEBUG ("OnIniTimer");
	robot_logic::RobotManager::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

static handler_t OnTimeOut (struct server *srv, char* id, 
							int opcode, int time){
    //LOG_DEBUG ("OnTimeout");
    robot_logic::RobotManager::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len){
	return HANDLER_GO_ON;
}

int robot_manager_plugin_init(struct plugin *pl){
    
	pl->init = OnRobotManagerStart;
	pl->clean_up = OnRobotManagerShutdown;
	pl->connection = OnRobotManagerConnect;
	pl->connection_close = OnRobotCenterConectClose;
	pl->connection_close_srv = OnBroadcastClose;
	pl->connection_srv = OnBroadcastConnect;
	pl->handler_init_time = OnIniTimer;
	pl->handler_read = OnRobotCenterMessage;
	pl->handler_read_srv = OnBroadcastMessage;
	pl->handler_read_other = OnUnknow;
	pl->time_msg = OnTimeOut;
	pl->data = NULL;
	return 0;

}
