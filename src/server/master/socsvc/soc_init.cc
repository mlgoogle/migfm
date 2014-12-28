#include "soc_init.h"
#include "soc_logic.h"
#include "common.h"
#include "plugins.h"


struct socplugin{
    char* id;
    char* name;
    char* version;
    char* provider;
};


static void *OnSocStart(){

    signal(SIGPIPE,SIG_IGN);
    struct socplugin* soc=(struct socplugin*)calloc(1,sizeof(struct socplugin));
    soc->id="soc";

    soc->name="soc";

    soc->version="1.0";

    soc->provider="kerry";

    if(!socsvc_logic::Soclogic::GetInstance())
        assert(0);

    return soc ;

}

static handler_t OnSocShutdown(struct server* srv,void* pd){

    socsvc_logic::Soclogic::FreeInstance();

    return HANDLER_GO_ON;
}

static handler_t OnSocConnect(struct server *srv, int fd, void *data, int len){

    socsvc_logic::Soclogic::GetInstance()->OnSocConnect(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnSocMessage(struct server *srv, int fd, void *data, int len){

    socsvc_logic::Soclogic::GetInstance()->OnSocMessage(srv,fd,data,len);

    return HANDLER_GO_ON;
}

static handler_t OnSocClose(struct server *srv, int fd){

    socsvc_logic::Soclogic::GetInstance()->OnSocClose(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len){


    return HANDLER_GO_ON;

}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void *data, int len){


    socsvc_logic::Soclogic::GetInstance()->OnBroadcastConnect(srv,fd,data,len);

    return HANDLER_GO_ON;

}

static handler_t OnBroadcastClose(struct server* srv, int fd){ 


    socsvc_logic::Soclogic::GetInstance()->OnBroadcastClose(srv,fd);

    return HANDLER_GO_ON;


}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data, int len){


    socsvc_logic::Soclogic::GetInstance()->OnBroadcastMessage(srv,fd,data,len);

    return HANDLER_GO_ON;


}

static handler_t OnIniTimer(struct server* srv){ 


    socsvc_logic::Soclogic::GetInstance()->OnIniTimer(srv);

    return HANDLER_GO_ON;


}

static handler_t OnTimeOut(struct server* srv,char* id, int opcode, int time){ 


    socsvc_logic::Soclogic::GetInstance()->OnTimeout(srv,id,opcode,time);

    return HANDLER_GO_ON;


}






int soc_plugin_init(struct plugin *pl){


    pl->init=OnSocStart;

    pl->clean_up = OnSocShutdown;

    pl->connection = OnSocConnect;

    pl->connection_close = OnSocClose;

    pl->connection_close_srv = OnBroadcastClose;

    pl->connection_srv = OnBroadcastConnect;

    pl->handler_init_time = OnIniTimer;

    pl->handler_read = OnSocMessage;

    pl->handler_read_srv = OnBroadcastMessage;

    pl->handler_read_other = OnUnknow;

    pl->time_msg = OnTimeOut;

    pl->data = NULL;

    return 0;

}

