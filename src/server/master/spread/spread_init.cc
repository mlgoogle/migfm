#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "spread_init.h"
#include "spread_engine.h"
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

static void* spread_init(){

	LOG_DEBUG("spread_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"spread");
    CLEAN(name,"spread");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"kerry");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (spread_logic::SpreadEngine::GetInstance() == NULL)
		assert(0);

	
    return static_cast<void*>(pl_data);
}

static handler_t  spread_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("spread_clean_up");
    spread_logic::SpreadEngine::FreeInstance();
	LOG_MSG("SpreadEngine was shutdown");

    return HANDLER_GO_ON;
}

static handler_t spread_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("spread_connection");
	spread_logic::SpreadEngine::GetInstance()->OnSpreadConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t spread_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("spread_read");
	spread_logic::SpreadEngine::GetInstance()->OnSpreadMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t spread_connection_close(struct server *srv,int fd){
	LOG_DEBUG("spread_connection_close");
	spread_logic::SpreadEngine::GetInstance()->OnSpreadClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t spread_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("spread_connection_srv");
	spread_logic::SpreadEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t spread_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("spread_read_srv");
    spread_logic::SpreadEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
} 

static handler_t spread_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("spread_connection_close_srv");
	spread_logic::SpreadEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t spread_time_msg(struct server* srv,char* id,int opcode,int time){

	LOG_DEBUG("spread_time_msg");
	spread_logic::SpreadEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t spread_init_time(struct server* srv){
	LOG_DEBUG("spread_init_time");
	spread_logic::SpreadEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int spread_plugin_init(struct plugin* pl)
{
    pl->init = spread_init;
    pl->clean_up = spread_clean_up;
    pl->handler_read = spread_read;
    pl->connection = spread_connection;
    pl->connection_srv = spread_connection_srv;
    pl->handler_read_srv = spread_read_srv;
    pl->connection_close = spread_connection_close;
    pl->connection_close_srv = spread_connection_close_srv;
	pl->time_msg = spread_time_msg;
	pl->handler_init_time = spread_init_time;
    pl->data = NULL;
    return 0;
}
