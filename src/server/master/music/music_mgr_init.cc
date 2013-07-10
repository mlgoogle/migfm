#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include "common.h"
#include "plugins.h"
#include "music_mgr_init.h"
#include "music_mgr_engine.h"
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

static void* music_manager_init(){

	LOG_DEBUG("music_mgr_init");
	signal (SIGPIPE, SIG_IGN);
    struct plugin_data* pl_data;
    pl_data = new struct plugin_data;

#define CLEAN(x,y)\
    pl_data->x = new char[strlen(y)+1];\
    strcpy(pl_data->x,y);

    CLEAN(id,"music_manager");
    CLEAN(name,"music_manager");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"kerry");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	if (music_logic::MusicMgrEngine::GetInstance() == NULL)
		assert(0);

	LOG_MSG("UsrMgrEngine was started...");
	
    return static_cast<void*>(pl_data);
}

static handler_t  music_manager_clean_up(struct server *srv,void* pd_t){
    LOG_DEBUG("music_mgr_clean_up");
	music_logic::MusicMgrEngine::FreeInstance();
	LOG_MSG("MusicMgrEngine was shutdown");

    return HANDLER_GO_ON;
}

static handler_t music_manager_connection(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("music_mgr_connection");
	music_logic::MusicMgrEngine::GetInstance()->OnMusicMgrConnect(srv,fd);
    return HANDLER_GO_ON;
}


static handler_t music_manager_read(struct server *srv,int fd,void *pd,int len){
	LOG_DEBUG("music_mgr_read");
	music_logic::MusicMgrEngine::GetInstance()->OnMusicMgrMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t music_manager_connection_close(struct server *srv,int fd){
	LOG_DEBUG("music_mgr_connection_close");
	music_logic::MusicMgrEngine::GetInstance()->OnMusicMgrClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t music_manager_connection_srv(struct server *srv,int fd,void *pd,
                                        int len){
	LOG_DEBUG("music_mgr_connection_srv");
	music_logic::MusicMgrEngine::GetInstance()->OnBroadcastConnect(srv,fd,pd,len);
    return HANDLER_GO_ON;
}

static handler_t music_manager_read_srv(struct server *srv,int fd,void *pd,
		                             int len){
    LOG_DEBUG("music_mgr_read_srv");
	music_logic::MusicMgrEngine::GetInstance()->OnBroadcastMessage(srv,fd,pd,len);
    return HANDLER_GO_ON;
} 

static handler_t music_manager_connection_close_srv(struct server* srv,int fd){
	LOG_DEBUG("music_mgr_connection_close_srv");
	music_logic::MusicMgrEngine::GetInstance()->OnBroadcastClose(srv,fd);
    return HANDLER_GO_ON;
}

static handler_t music_manager_time_msg(struct server* srv,char* id,int opcode,int time){

	LOG_DEBUG("music_mgr_time_msg");
	music_logic::MusicMgrEngine::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t music_manager_init_time(struct server* srv){
	LOG_DEBUG("music_mgr_init_time");
	music_logic::MusicMgrEngine::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int music_manager_plugin_init(struct plugin* pl)
{
    pl->init = music_manager_init;
    pl->clean_up = music_manager_clean_up;
    pl->handler_read = music_manager_read; 
    pl->connection = music_manager_connection;
    pl->connection_srv = music_manager_connection_srv;
    pl->handler_read_srv = music_manager_read_srv;
    pl->connection_close = music_manager_connection_close;
    pl->connection_close_srv = music_manager_connection_close_srv;
	pl->time_msg = music_manager_time_msg;
	pl->handler_init_time = music_manager_init_time;
    pl->data = NULL;
    return 0;
}
