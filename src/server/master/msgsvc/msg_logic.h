#ifndef __MIGFM__MSGSVC__MSG_LOGIC___
#define __MIGFM__MSGSVC__MSG_LOGIC___
#include "common.h"


namespace msgsvc_logic{

class Msglogic{

public:
    Msglogic();
    virtual ~Msglogic();

private:
    static Msglogic    *instance_;

public:
    static Msglogic *GetInstance();
    static void FreeInstance();

public:
    bool OnMsgConnect (struct server *srv,const int socket);

    bool OnMsgMessage (struct server *srv, const int socket,const void *msg, const int len);

    bool OnMsgClose (struct server *srv,const int socket);

    bool OnBroadcastConnect(struct server *srv,const int socket,const void *data, const int len);

    bool OnBroadcastMessage (struct server *srv, const int socket, const void *msg, const int len);

    bool OnBroadcastClose (struct server *srv, const int socket);

    bool OnIniTimer (struct server *srv);

    bool OnTimeout (struct server *srv, char* id, int opcode, int time);


private:

    bool GetPushMessageQueue();


    bool Init();
};


}

#endif

