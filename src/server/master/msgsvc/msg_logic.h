#ifndef __MIGFM__MSGSVC__MSG_LOGIC___
#define __MIGFM__MSGSVC__MSG_LOGIC___
#include "queue/block_msg_queue.h"
#include "logic/cache_manager.h"
#include "common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/msgsvc/msgsvc_config.xml"
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

    void GetPushMessageQueue();

    void ResolveGiving(base_queue::BlockMsg* block);

    void ResolveSayHello(base_queue::BlockMsg* block);

    void PushMessage(const int64 uid,const int64 tid,const std::string& message);

    template <typename ELEMENT>
    void ResolveTemplate(base_queue::BlockMsg* block,int64& uid,int64& tid,
    		std::list<ELEMENT>& list);

    bool Init();
};


}

#endif

