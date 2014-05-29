#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "identity_provider_engine.h"
#include "identity_provider_init.h"
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

static void* sso_init(){

	LOG_DEBUG("identity_provider_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"identity_provider");
    CLEAN(name,"identity_provider");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"kerry");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (sso_logic::IdentityProviderEngine::GetInstance() == NULL)
		assert(0);

	LOG_MSG("SSO was started...");
	
    return static_cast<void*>(pl_data);
}

static handler_t  identity_provider_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("identity_provider_clean_up");
	sso_logic::IdentityProviderEngine::FreeInstance();
	LOG_MSG("identity_provider was shutdown");

    return HANDLER_GO_ON;
}

static handler_t identity_provider_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("identity_provider_connection");
	sso_logic::IdentityProviderEngine::GetInstance()->OnConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t identity_provider_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("identity_provider_read");
	sso_logic::IdentityProviderEngine::GetInstance()->OnMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t identity_provider_connection_close(struct server *srv,int fd){
	LOG_DEBUG("identity_provider_connection_close");
	sso_logic::IdentityProviderEngine::GetInstance()->OnClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t identity_provider_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("identity_provider_connection_srv");
	sso_logic::IdentityProviderEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t identity_provider_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("identity_provider_read_srv");
	sso_logic::IdentityProviderEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
} 

static handler_t identity_provider_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("identity_provider_connection_close_srv");
	sso_logic::IdentityProviderEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t identity_provider_time_msg(struct server* srv,char* id,
										   int opcode,int time){

	LOG_DEBUG("identity_provider_time_msg");
	sso_logic::IdentityProviderEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t identity_provider_init_time(struct server* srv){
	LOG_DEBUG("identity_provider_init_time");
	sso_logic::IdentityProviderEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int user_manager_plugin_init(struct plugin* pl)
{
    pl->init = sso_init;
    pl->clean_up = identity_provider_clean_up;
    pl->handler_read = identity_provider_read; 
    pl->connection = identity_provider_connection;
    pl->connection_srv = identity_provider_connection_srv;
    pl->handler_read_srv = identity_provider_read_srv;
    pl->connection_close = identity_provider_connection_close;
    pl->connection_close_srv = identity_provider_connection_close_srv;
	pl->time_msg = identity_provider_time_msg;
	pl->handler_init_time = identity_provider_init_time;
    pl->data = NULL;
    return 0;
}
