#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "robot_init.h"
#include "robot_engine.h"
#include "logic_comm.h"

static bool dumpCallback(const char* dump_path,
		         const char* minidump_id,
			 void* context,
			 bool succeeded){
    MIG_ERROR(USER_LEVEL,"Dump path:%s/%s.dump\n",dump_path,minidump_id);
    return succeeded;
}

struct plugin_data{

    PLUGIN_DATA_ID;

    PLUGIN_DATA_NAME;

    PLUGIN_DATA_VERSION;

    PLUGIN_DATA_PROVIDER;

};

static void time_call_back(void* arg){
    fprintf(stderr,"id id id \n");
}

static void* robot_init(){

	LOG_DEBUG("robot_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"robot");
    CLEAN(name,"robot");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"kerry");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (robot_logic::RobotEngine::GetInstance() == NULL)
		assert(0);

	LOG_MSG("robot::RobotEngine was started...");

    return static_cast<void*>(pl_data);
}

static handler_t  robot_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("robot_clean_up");
    robot_logic::RobotEngine::FreeInstance();
	LOG_MSG("robot::RobotEngine was shutdown");

    return HANDLER_GO_ON;
}

static handler_t robot_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("robot_connection");
	robot_logic::RobotEngine::GetInstance()->OnRobotConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t robot_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("robot_read");
	robot_logic::RobotEngine::GetInstance()->OnRobotMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t robot_connection_close(struct server *srv,int fd){
	LOG_DEBUG("robot_connection_close");
	robot_logic::RobotEngine::GetInstance()->OnRobotClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t robot_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("robot_connection_srv");
	robot_logic::RobotEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t robot_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("robot_read_srv");
    robot_logic::RobotEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t robot_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("robot_connection_close_srv");
	robot_logic::RobotEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t robot_time_msg(struct server* srv,char* id,int opcode,int time){

	LOG_DEBUG("robot_time_msg");
	robot_logic::RobotEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t robot_init_time(struct server* srv){
	LOG_DEBUG("robot_init_time");
	robot_logic::RobotEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int robot_plugin_init(struct plugin* pl)
{
    pl->init = robot_init;
    pl->clean_up = robot_clean_up;
    pl->handler_read = robot_read;
    pl->connection = robot_connection;
    pl->connection_srv = robot_connection_srv;
    pl->handler_read_srv = robot_read_srv;
    pl->connection_close = robot_connection_close;
    pl->connection_close_srv = robot_connection_close_srv;
	pl->time_msg = robot_time_msg;
	pl->handler_init_time = robot_init_time;
    pl->data = NULL;
    return 0;
}
