#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "usr_mgr_init.h"
#include "usr_mgr_engine.h"
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

static void* user_manager_init(){

	LOG_DEBUG("usr_mgr_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"user_manager");
    CLEAN(name,"user_manager");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"kerry");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (usr_logic::UsrMgrEngine::GetInstance() == NULL)
		assert(0);

	LOG_MSG("UsrMgrEngine was started...");
	
    return static_cast<void*>(pl_data);
}

static handler_t  user_manager_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("usr_mgr_clean_up");
	usr_logic::UsrMgrEngine::FreeInstance();
	LOG_MSG("UsrMgrEngine was shutdown");

    return HANDLER_GO_ON;
}

static handler_t user_manager_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("usr_mgr_connection");
	usr_logic::UsrMgrEngine::GetInstance()->OnUsrMgrConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t user_manager_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("usr_mgr_read");
	usr_logic::UsrMgrEngine::GetInstance()->OnUsrMgrMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t user_manager_connection_close(struct server *srv,int fd){
	LOG_DEBUG("usr_mgr_connection_close");
	usr_logic::UsrMgrEngine::GetInstance()->OnUsrMgrClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t user_manager_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("usr_mgr_connection_srv");
	usr_logic::UsrMgrEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t user_manager_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("usr_mgr_read_srv");
	usr_logic::UsrMgrEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
} 

static handler_t user_manager_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("usr_mgr_connection_close_srv");
	usr_logic::UsrMgrEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t user_manager_time_msg(struct server* srv,char* id,int opcode,int time){

	LOG_DEBUG("usr_mgr_time_msg");
	usr_logic::UsrMgrEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t user_manager_init_time(struct server* srv){
	LOG_DEBUG("usr_mgr_init_time");
	usr_logic::UsrMgrEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int user_manager_plugin_init(struct plugin* pl)
{
    pl->init = user_manager_init;
    pl->clean_up = user_manager_clean_up;
    pl->handler_read = user_manager_read; 
    pl->connection = user_manager_connection;
    pl->connection_srv = user_manager_connection_srv;
    pl->handler_read_srv = user_manager_read_srv;
    pl->connection_close = user_manager_connection_close;
    pl->connection_close_srv = user_manager_connection_close_srv;
	pl->time_msg = user_manager_time_msg;
	pl->handler_init_time = user_manager_init_time;
    pl->data = NULL;
    return 0;
}
