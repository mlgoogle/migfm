#include "msg_init.h"
#include "msg_logic.h"
#include "common.h"
#include "plugins.h"


struct msgplugin{
    char* id;
    char* name;
    char* version;
    char* provider;
};


static void *OnMsgStart(){

    signal(SIGPIPE,SIG_IGN);
    struct msgplugin* msg=(struct msgplugin*)calloc(1,sizeof(struct msgplugin));

    msg->id="msg";

    msg->name="msg";

    msg->version="1.0.0";

    msg->provider="kerry";

    if(!msgsvc_logic::Msglogic::GetInstance())
        assert(0);

    return msg ;

}

static handler_t OnMsgShutdown(struct server* srv,void* pd){

    msgsvc_logic::Msglogic::FreeInstance();

    return HANDLER_GO_ON;
}

static handler_t OnMsgConnect(struct server *srv, int fd, void *data, int len){

    msgsvc_logic::Msglogic::GetInstance()->OnMsgConnect(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnMsgMessage(struct server *srv, int fd, void *data, int len){

    msgsvc_logic::Msglogic::GetInstance()->OnMsgMessage(srv,fd,data,len);

    return HANDLER_GO_ON;
}

static handler_t OnMsgClose(struct server *srv, int fd){

    msgsvc_logic::Msglogic::GetInstance()->OnMsgClose(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len){


    return HANDLER_GO_ON;

}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void *data, int len){


    msgsvc_logic::Msglogic::GetInstance()->OnBroadcastConnect(srv,fd,data,len);

    return HANDLER_GO_ON;

}

static handler_t OnBroadcastClose(struct server* srv, int fd){ 


    msgsvc_logic::Msglogic::GetInstance()->OnBroadcastClose(srv,fd);

    return HANDLER_GO_ON;


}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data, int len){


    msgsvc_logic::Msglogic::GetInstance()->OnBroadcastMessage(srv,fd,data,len);

    return HANDLER_GO_ON;


}

static handler_t OnIniTimer(struct server* srv){ 


    msgsvc_logic::Msglogic::GetInstance()->OnIniTimer(srv);

    return HANDLER_GO_ON;


}

static handler_t OnTimeOut(struct server* srv,char* id, int opcode, int time){ 


    msgsvc_logic::Msglogic::GetInstance()->OnTimeout(srv,id,opcode,time);

    return HANDLER_GO_ON;


}






int msg_plugin_init(struct plugin *pl){


    pl->init=OnMsgStart;

    pl->clean_up = OnMsgShutdown;

    pl->connection = OnMsgConnect;

    pl->connection_close = OnMsgClose;

    pl->connection_close_srv = OnBroadcastClose;

    pl->connection_srv = OnBroadcastConnect;

    pl->handler_init_time = OnIniTimer;

    pl->handler_read = OnMsgMessage;

    pl->handler_read_srv = OnBroadcastMessage;

    pl->handler_read_other = OnUnknow;

    pl->time_msg = OnTimeOut;

    pl->data = NULL;

    return 0;

}

