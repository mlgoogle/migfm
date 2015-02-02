#ifndef __MIGFM__SOCSVC__SOC_LOGIC___
#define __MIGFM__SOCSVC__SOC_LOGIC___
#include "net/soc_comm_head.h"
#include "common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/socsvc/socsvc_config.xml"
namespace socsvc_logic{

class Soclogic{

public:
    Soclogic();
    virtual ~Soclogic();

private:
    static Soclogic    *instance_;

public:
    static Soclogic *GetInstance();
    static void FreeInstance();

public:
    bool OnSocConnect (struct server *srv,const int socket);

    bool OnSocMessage (struct server *srv, const int socket,const void *msg, const int len);

    bool OnSocClose (struct server *srv,const int socket);

    bool OnBroadcastConnect(struct server *srv,const int socket,const void *data, const int len);

    bool OnBroadcastMessage (struct server *srv, const int socket, const void *msg, const int len);

    bool OnBroadcastClose (struct server *srv, const int socket);

    bool OnIniTimer (struct server *srv);

    bool OnTimeout (struct server *srv, char* id, int opcode, int time);
private:
    bool OnGainBarrageComm(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
        		const void* msg = NULL,const int len = 0);

    bool OnGainLocation(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnSayHello(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnGivingSong(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);
public:
    void AddMoreGivingSongBlockMessage(int64 uid,int64 tid,std::string& json_str);

private:

    bool Init();
};


}

#endif

