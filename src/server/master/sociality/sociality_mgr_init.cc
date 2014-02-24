#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "sociality_mgr_init.h"
#include "sociality_mgr_engine.h"
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

static void* sociality_manager_init(){

	LOG_DEBUG("sociality_mgr_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"sociality_manager");
    CLEAN(name,"sociality_manager");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"hy");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (mig_sociality::SocialityMgrEngine::GetInstance() == NULL)
		assert(0);

	LOG_MSG("SocialityMgrEngine was started...");
	
    return static_cast<void*>(pl_data);
}

static handler_t  sociality_manager_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("sociality_mgr_clean_up");
	mig_sociality::SocialityMgrEngine::FreeInstance();
	LOG_MSG("SocialityMgrEngine was shutdown");

    return HANDLER_GO_ON;
}

static handler_t sociality_manager_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("sociality_mgr_connection");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t sociality_manager_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("sociality_mgr_read");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnReadMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t sociality_manager_connection_close(struct server *srv,int fd){
	LOG_DEBUG("sociality_mgr_connection_close");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t sociality_manager_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("sociality_mgr_connection_srv");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t sociality_manager_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("sociality_mgr_read_srv");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
} 

static handler_t sociality_manager_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("sociality_mgr_connection_close_srv");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t sociality_manager_time_msg(struct server* srv,char* id,int opcode,int time){

	LOG_DEBUG("sociality_mgr_time_msg");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t sociality_manager_init_time(struct server* srv){
	LOG_DEBUG("sociality_mgr_init_time");
	mig_sociality::SocialityMgrEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int sociality_manager_plugin_init(struct plugin* pl)
{
    pl->init = sociality_manager_init;
    pl->clean_up = sociality_manager_clean_up;
    pl->handler_read = sociality_manager_read;
    pl->connection = sociality_manager_connection;
    pl->connection_srv = sociality_manager_connection_srv;
    pl->handler_read_srv = sociality_manager_read_srv;
    pl->connection_close = sociality_manager_connection_close;
    pl->connection_close_srv = sociality_manager_connection_close_srv;
	pl->time_msg = sociality_manager_time_msg;
	pl->handler_init_time = sociality_manager_init_time;
    pl->data = NULL;
    return 0;
}
