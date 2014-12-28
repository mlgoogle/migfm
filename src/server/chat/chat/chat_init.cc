#include "chat_init.h"
#include "chat_manager.h"
#include "logic_unit.h"
#include "common.h"
#include "plugins.h"
#include "base/logic_comm.h"
#include "logic/logic_comm.h"


struct ChatManagerPlugin{
	char* id;
	char* name;
	char* version;
	char* provider;
};

static void *OnChatManagerStart(){
    LOG_DEBUG("OnChatManagerStart");
	signal(SIGPIPE,SIG_IGN);
	struct ChatManagerPlugin* chat_mgr
		= (struct ChatManagerPlugin*) calloc(1,sizeof(struct ChatManagerPlugin));
	chat_mgr->id = PLUGIN_ID;
	chat_mgr->name = PLUGIN_NAME;
	chat_mgr->version = PLUGIN_VERSION;
	chat_mgr->provider = PLUGIN_PROVIDER;

	if (!chat_logic::ChatManager::GetInstance())
		assert(0);

	LOG_MSG("ChatManagerPlugin was started");
	return chat_mgr;
}

static handler_t OnChatManagerShutdown(struct server* srv,void* pd){
	chat_logic::ChatManager::FreeInstance();
	LOG_MSG("OnChatManagerShutdown was shutdown");
	return HANDLER_GO_ON;
}

static handler_t OnChatManagerConnect(struct server *srv, int fd, 
										void *data, int len){

	LOG_DEBUG("OnChatManagerConnect");
	chat_logic::ChatManager::GetInstance()->OnChatConnect(srv,fd);
	return HANDLER_GO_ON;
}

static handler_t OnChatCenterConectClose(struct server* srv,int fd){
	
	LOG_DEBUG("OnChatCenterConectClose");
	chat_logic::ChatManager::GetInstance()->OnChatManagerClose(srv,fd);
	return HANDLER_GO_ON;
}

static handler_t OnChatCenterMessage(struct server *srv, int sock, 
										void *data, int len){

	LOG_DEBUG("OnChatCenterMessage");
	chat_logic::ChatManager::GetInstance()->OnChatManagerMessage(srv,sock,data,len);
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
	chat_logic::ChatManager::GetInstance()->OnBroadcastConnect(srv,fd,data,len);
	return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose (struct server *srv, int fd)
{
	LOG_DEBUG ("OnBroadcastClose");
	chat_logic::ChatManager::GetInstance()->OnBroadcastClose(srv,fd);
	return HANDLER_GO_ON;

}
static handler_t OnBroadcastMessage (struct server *srv, int fd, void *data, int len)
{
	LOG_DEBUG ("OnBroadcastMessage");
	chat_logic::ChatManager::GetInstance()->OnBroadcastMessage(srv,fd,data,len);
	return HANDLER_GO_ON;
}

static handler_t OnClock (struct server *srv, int fd)
{
	LOG_DEBUG ("OnClock");
	return HANDLER_GO_ON;
}

static handler_t OnIniTimer (struct server *srv){
	LOG_DEBUG ("OnIniTimer");
	chat_logic::ChatManager::GetInstance()->OnIniTimer(srv);
	return HANDLER_GO_ON;
}

static handler_t OnTimeOut (struct server *srv, char* id, 
							int opcode, int time){
    LOG_DEBUG ("OnTimeout");
	chat_logic::ChatManager::GetInstance()->OnTimeout(srv,id,opcode,time);
	return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len){
	return HANDLER_GO_ON;
}

int chat_manager_plugin_init(struct plugin *pl){
    
	pl->init = OnChatManagerStart;
	pl->clean_up = OnChatManagerShutdown;
	pl->connection = OnChatManagerConnect;
	pl->connection_close = OnChatCenterConectClose;
	pl->connection_close_srv = OnBroadcastClose;
	pl->connection_srv = OnBroadcastConnect;
	pl->handler_init_time = OnIniTimer;
	pl->handler_read = OnChatCenterMessage;
	pl->handler_read_srv = OnBroadcastMessage;
	pl->handler_read_other = OnUnknow;
	pl->time_msg = OnTimeOut;
	pl->data = NULL;
	return 0;

}
