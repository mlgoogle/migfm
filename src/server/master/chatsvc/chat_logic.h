#ifndef __MIGFM__CHATSVC__CHAT_LOGIC___
#define __MIGFM__CHATSVC__CHAT_LOGIC___
#include "net/chat_comm_head.h"
#include "common.h"
#define DEFAULT_CONFIG_PATH     "./plugins/chatsvc/chatsvc_config.xml"

namespace chatsvc_logic{

class Chatlogic{

public:
    Chatlogic();
    virtual ~Chatlogic();

private:
    static Chatlogic    *instance_;

public:
    static Chatlogic *GetInstance();
    static void FreeInstance();

public:
    bool OnChatConnect (struct server *srv,const int socket);

    bool OnChatMessage (struct server *srv, const int socket,const void *msg, const int len);

    bool OnChatClose (struct server *srv,const int socket);

    bool OnBroadcastConnect(struct server *srv,const int socket,const void *data, const int len);

    bool OnBroadcastMessage (struct server *srv, const int socket, const void *msg, const int len);

    bool OnBroadcastClose (struct server *srv, const int socket);

    bool OnIniTimer (struct server *srv);

    bool OnTimeout (struct server *srv, char* id, int opcode, int time);

public:
    bool OnGainGroupMessage(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
          		const void* msg = NULL,const int len = 0);

    bool OnGainLeaveMessage(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
      		const void* msg = NULL,const int len = 0);

    bool OnGainIdleChatSvc(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
         		const void* msg = NULL,const int len = 0);
private:

    bool Init();
};


}

#endif

