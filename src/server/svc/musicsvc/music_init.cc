#include "music_init.h"
#include "music_logic.h"
#include "common.h"
#include "plugins.h"


struct musicplugin{
    char* id;
    char* name;
    char* version;
    char* provider;
};


static void *OnMusicStart(){

    signal(SIGPIPE,SIG_IGN);
    struct musicplugin* music=(struct musicplugin*)calloc(1,sizeof(struct musicplugin));

    music->id="music";

    music->name="music";

    music->version="1.0.0";

    music->provider="kerry";

    if(!musicsvc_logic::Musiclogic::GetInstance())
        assert(0);

    return music ;

}

static handler_t OnMusicShutdown(struct server* srv,void* pd){

    musicsvc_logic::Musiclogic::FreeInstance();

    return HANDLER_GO_ON;
}

static handler_t OnMusicConnect(struct server *srv, int fd, void *data, int len){

    musicsvc_logic::Musiclogic::GetInstance()->OnMusicConnect(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnMusicMessage(struct server *srv, int fd, void *data, int len){

    musicsvc_logic::Musiclogic::GetInstance()->OnMusicMessage(srv,fd,data,len);

    return HANDLER_GO_ON;
}

static handler_t OnMusicClose(struct server *srv, int fd){

    musicsvc_logic::Musiclogic::GetInstance()->OnMusicClose(srv,fd);

    return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len){


    return HANDLER_GO_ON;

}

static handler_t OnBroadcastConnect(struct server* srv, int fd, void *data, int len){


    musicsvc_logic::Musiclogic::GetInstance()->OnBroadcastConnect(srv,fd,data,len);

    return HANDLER_GO_ON;

}

static handler_t OnBroadcastClose(struct server* srv, int fd){ 


    musicsvc_logic::Musiclogic::GetInstance()->OnBroadcastClose(srv,fd);

    return HANDLER_GO_ON;


}

static handler_t OnBroadcastMessage(struct server* srv, int fd, void *data, int len){


    musicsvc_logic::Musiclogic::GetInstance()->OnBroadcastMessage(srv,fd,data,len);

    return HANDLER_GO_ON;


}

static handler_t OnIniTimer(struct server* srv){ 


    musicsvc_logic::Musiclogic::GetInstance()->OnIniTimer(srv);

    return HANDLER_GO_ON;


}

static handler_t OnTimeOut(struct server* srv,char* id, int opcode, int time){ 


    musicsvc_logic::Musiclogic::GetInstance()->OnTimeout(srv,id,opcode,time);

    return HANDLER_GO_ON;


}






int music_plugin_init(struct plugin *pl){


    pl->init=OnMusicStart;

    pl->clean_up = OnMusicShutdown;

    pl->connection = OnMusicConnect;

    pl->connection_close = OnMusicClose;

    pl->connection_close_srv = OnBroadcastClose;

    pl->connection_srv = OnBroadcastConnect;

    pl->handler_init_time = OnIniTimer;

    pl->handler_read = OnMusicMessage;

    pl->handler_read_srv = OnBroadcastMessage;

    pl->handler_read_other = OnUnknow;

    pl->time_msg = OnTimeOut;

    pl->data = NULL;

    return 0;

}

