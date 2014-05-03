#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "manager_init.h"
#include "manager_engine.h"
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

static void* manager_init(){

	LOG_DEBUG("manager_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"manager");
    CLEAN(name,"manager");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"kerry");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (manager_logic::ManagerEngine::GetInstance() == NULL)
		assert(0);

	LOG_MSG("UsrMgrEngine was started...");
	
    return static_cast<void*>(pl_data);
}

static handler_t  manager_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("mgr_clean_up");
    manager_logic::ManagerEngine::FreeInstance();
	LOG_MSG("ManagerEngine was shutdown");

    return HANDLER_GO_ON;
}

static handler_t manager_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("manager_connection");
	manager_logic::ManagerEngine::GetInstance()->OnManagerConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t manager_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("manager_read");
	manager_logic::ManagerEngine::GetInstance()->OnManagerMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t manager_connection_close(struct server *srv,int fd){
	LOG_DEBUG("manager_connection_close");
	manager_logic::ManagerEngine::GetInstance()->OnManagerClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t manager_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("manager_connection_srv");
	manager_logic::ManagerEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t manager_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("manager_read_srv");
    manager_logic::ManagerEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
} 

static handler_t manager_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("manager_connection_close_srv");
	manager_logic::ManagerEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t manager_time_msg(struct server* srv,char* id,int opcode,int time){

	LOG_DEBUG("manager_time_msg");
	manager_logic::ManagerEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t manager_init_time(struct server* srv){
	LOG_DEBUG("manager_init_time");
	manager_logic::ManagerEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int manager_plugin_init(struct plugin* pl)
{
    pl->init = manager_init;
    pl->clean_up = manager_clean_up;
    pl->handler_read = manager_read;
    pl->connection = manager_connection;
    pl->connection_srv = manager_connection_srv;
    pl->handler_read_srv = manager_read_srv;
    pl->connection_close = manager_connection_close;
    pl->connection_close_srv = manager_connection_close_srv;
	pl->time_msg = manager_time_msg;
	pl->handler_init_time = manager_init_time;
    pl->data = NULL;
    return 0;
}
