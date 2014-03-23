#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "chat_mgr_init.h"
#include "chat_mgr_engine.h"
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

static void* chat_manager_init(){

	LOG_DEBUG("chat_mgr_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"chat_manager");
    CLEAN(name,"chat_manager");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"kerry");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (chat_logic::ChatMgrEngine::GetInstance() == NULL)
		assert(0);

	LOG_MSG("UsrMgrEngine was started...");
	
    return static_cast<void*>(pl_data);
}

static handler_t  chat_manager_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("chat_mgr_clean_up");
    chat_logic::ChatMgrEngine::FreeInstance();
	LOG_MSG("UsrMgrEngine was shutdown");

    return HANDLER_GO_ON;
}

static handler_t chat_manager_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("chat_mgr_connection");
	chat_logic::ChatMgrEngine::GetInstance()->OnChatMgrConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t chat_manager_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("chat_mgr_read");
	chat_logic::ChatMgrEngine::GetInstance()->OnChatMgrMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t chat_manager_connection_close(struct server *srv,int fd){
	LOG_DEBUG("chat_mgr_connection_close");
	chat_logic::ChatMgrEngine::GetInstance()->OnChatMgrClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t chat_manager_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("chat_mgr_connection_srv");
	chat_logic::ChatMgrEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t chat_manager_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("chat_mgr_read_srv");
    chat_logic::ChatMgrEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
} 

static handler_t chat_manager_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("chat_mgr_connection_close_srv");
	chat_logic::ChatMgrEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t chat_manager_time_msg(struct server* srv,char* id,int opcode,int time){

	LOG_DEBUG("chat_mgr_time_msg");
	chat_logic::ChatMgrEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t chat_manager_init_time(struct server* srv){
	LOG_DEBUG("chat_mgr_init_time");
	chat_logic::ChatMgrEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int chat_manager_plugin_init(struct plugin* pl)
{
    pl->init = chat_manager_init;
    pl->clean_up = chat_manager_clean_up;
    pl->handler_read = chat_manager_read;
    pl->connection = chat_manager_connection;
    pl->connection_srv = chat_manager_connection_srv;
    pl->handler_read_srv = chat_manager_read_srv;
    pl->connection_close = chat_manager_connection_close;
    pl->connection_close_srv = chat_manager_connection_close_srv;
	pl->time_msg = chat_manager_time_msg;
	pl->handler_init_time = chat_manager_init_time;
    pl->data = NULL;
    return 0;
}
