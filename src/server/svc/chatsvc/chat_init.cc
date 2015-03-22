#include "chat_init.h"
#include "chat_logic.h"
#include "common.h"
#include "plugins.h"


struct chatplugin{
    char* id;
    char* name;
    char* version;
    char* provider;
};


static void *OnChatStart(){

    signal(SIGPIPE,SIG_IGN);
    struct chatplugin* chat=(struct chatplugin*)calloc(1,sizeof(struct chatplugin));

    chat->id="chat";

    chat->name="chat";

    chat->version="1.0.0";

    chat->provider="kerry";

    if(!chatsvc_logic::Chatlogic::GetInstance())
        assert(0);

    return chat ;

}

static handler_t OnChatShutdown(struct server* srv,void* pd){

    chatsvc_logic::Chatlogic::FreeInstance();

    return HANDLER_GO_ON;
}

static handler_t OnChatConnect(struct server *srv, int fd, void *data, int len){

    chatsvc_logic::Chatlogic::GetInstance()->OnChatConnect(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnChatMessage(struct server *srv, int fd, void *data, int len){

    chatsvc_logic::Chatlogic::GetInstance()->OnChatMessage(srv,fd,data,len);

    return HANDLER_GO_ON;
}

static handler_t OnChatClose(struct server *srv, int fd){

    chatsvc_logic::Chatlogic::GetInstance()->OnChatClose(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len){


    return HANDLER_GO_ON;

}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void *data, int len){


    chatsvc_logic::Chatlogic::GetInstance()->OnBroadcastConnect(srv,fd,data,len);

    return HANDLER_GO_ON;

}

static handler_t OnBroadcastClose(struct server* srv, int fd){ 


    chatsvc_logic::Chatlogic::GetInstance()->OnBroadcastClose(srv,fd);

    return HANDLER_GO_ON;


}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data, int len){


    chatsvc_logic::Chatlogic::GetInstance()->OnBroadcastMessage(srv,fd,data,len);

    return HANDLER_GO_ON;


}

static handler_t OnIniTimer(struct server* srv){ 


    chatsvc_logic::Chatlogic::GetInstance()->OnIniTimer(srv);

    return HANDLER_GO_ON;


}

static handler_t OnTimeOut(struct server* srv,char* id, int opcode, int time){ 


    chatsvc_logic::Chatlogic::GetInstance()->OnTimeout(srv,id,opcode,time);

    return HANDLER_GO_ON;


}






int chat_plugin_init(struct plugin *pl){


    pl->init=OnChatStart;

    pl->clean_up = OnChatShutdown;

    pl->connection = OnChatConnect;

    pl->connection_close = OnChatClose;

    pl->connection_close_srv = OnBroadcastClose;

    pl->connection_srv = OnBroadcastConnect;

    pl->handler_init_time = OnIniTimer;

    pl->handler_read = OnChatMessage;

    pl->handler_read_srv = OnBroadcastMessage;

    pl->handler_read_other = OnUnknow;

    pl->time_msg = OnTimeOut;

    pl->data = NULL;

    return 0;

}

