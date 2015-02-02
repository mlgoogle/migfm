#ifndef __MIGFM__MUSICSVC__MUSIC_LOGIC___
#define __MIGFM__MUSICSVC__MUSIC_LOGIC___
#include "net/music_comm_head.h"
#include "common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/musicsvc/musicsvc_config.xml"
namespace musicsvc_logic{

class Musiclogic{

public:
    Musiclogic();
    virtual ~Musiclogic();

private:
    static Musiclogic    *instance_;

public:
    static Musiclogic *GetInstance();
    static void FreeInstance();

public:
    bool OnMusicConnect (struct server *srv,const int socket);

    bool OnMusicMessage (struct server *srv, const int socket,const void *msg, const int len);

    bool OnMusicClose (struct server *srv,const int socket);

    bool OnBroadcastConnect(struct server *srv,const int socket,const void *data, const int len);

    bool OnBroadcastMessage (struct server *srv, const int socket, const void *msg, const int len);

    bool OnBroadcastClose (struct server *srv, const int socket);

    bool OnIniTimer (struct server *srv);

    bool OnTimeout (struct server *srv, char* id, int opcode, int time);

private:

    bool OnCollectList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg = NULL,const int len = 0);

    bool OnHateList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg = NULL,const int len = 0);

private:

    bool Init();
};


}

#endif

