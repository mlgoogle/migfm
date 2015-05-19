#ifndef __MIGFM__MUSICSVC__MUSIC_LOGIC___
#define __MIGFM__MUSICSVC__MUSIC_LOGIC___
#include "net/music_comm_head.h"
#include "logic/logic_infos.h"
#include "common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/musicsvc/musicsvc_config.xml"

enum GETMUSICUSER{
	MUSIC_FRI = 0,
	MUSIC_NEAR = 1
};
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

    bool OnSetCollection(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg = NULL,const int len = 0);

    bool OnDelCollection(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnHateList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg = NULL,const int len = 0);

    bool OnDimensionList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg = NULL,const int len = 0);

    bool OnDimensionsList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnNearMusic(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnSocNearUser(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnRecordMusic(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnSocMyMusicFriend(struct server* srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnDimensionInfo(struct server* srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

    bool OnSocGetPushMessage(struct server* srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg = NULL,const int len = 0);

private:

    void GetNearUserAndMusic(const int64 uid,
    		const int32 cat,const double latitude,
    		const double longitude,
    		const int32 from,const int32 count,
    		std::list<base_logic::UserAndMusic>& infolist);

    bool Init();
private:
private:
    int                                              robot_server_socket_;
};


}

#endif

