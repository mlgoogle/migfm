#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"plugins.h"
#include"log/mig_log.h"
#include "baidu_lbs_connector.h"
#include "redis_connector.h"

#ifdef __cplusplus 
extern "C" {
#endif

int lbs_plugin_init (struct plugin *pl);

#ifdef __cplusplus 
}
#endif

struct plugin_data{

    PLUGIN_DATA_ID;

    PLUGIN_DATA_NAME;

    PLUGIN_DATA_VERSION;

    PLUGIN_DATA_PROVIDER;

};

static void* on_init()
{
    struct plugin_data* pl_data;
    pl_data = (struct plugin_data *)calloc(1,sizeof(*pl_data));

#define CLEAN(x,y)\
    pl_data->x = (char *)malloc(512);\
    sprintf(pl_data->x,y);

    CLEAN(id,"lbs");
    CLEAN(name,"lbs");
    CLEAN(version,"1.0.0");
    CLEAN(provider,"hy");
#undef CLEAN
    printf("id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);
	MIG_INFO(USER_LEVEL, "id:[%s] version[%s] %s\n",pl_data->id,pl_data->version,__FUNCTION__);

	mig_lbs::RedisConnector::GetInstance()->Connect();

	using namespace mig_lbs;
	BaiduLBSConnector conn;
//	conn.SetPOI(100001, 120.116506, 30.295979, "data100001");
//	conn.SetPOI(100002, 120.106506, 30.205979, "data100002");
//	conn.SetPOI(100003, 120.126506, 30.294979, "data100003");
//	conn.SetPOI(100004, 120.118506, 30.293979, "data100004");
//	conn.SetPOI(100005, 120.112506, 30.298979, "data100005");

	conn.SearchNearby(120.116506, 30.295979, 10000, "");
    return pl_data;
}

static handler_t  on_clean_up(struct server *srv,void* pd_t)
{
	mig_lbs::RedisConnector::FreeInstance();
    return HANDLER_GO_ON;
}

static handler_t on_connection(struct server *srv,int fd,void *pd,int len)
{
	printf("new connection:%x!\n", fd);

	MIG_INFO(USER_LEVEL, "new connection:%x", fd);
	return HANDLER_GO_ON;
}

static handler_t on_read(struct server *srv,int fd,void *pd,int len)
{
	//printf("%*.*s\n",len-4,len-4,(char*)pd+4);
	//    struct netaio *aio;
	//    struct socket_adapter *sca;
	int rc;

	MIG_INFO(USER_LEVEL, "recv data:%s", (char *)pd);
	if (-1 == send(fd, pd, len, 0)) {
		MIG_INFO(USER_LEVEL, "send failed: %s", strerror(errno));
	}
	return HANDLER_GO_ON;
}

__attribute__((visibility("default")))
int lbs_plugin_init(struct plugin* pl)
{
	pl->init = on_init;
	pl->clean_up = on_clean_up;
	pl->handler_read = on_read; 
	pl->connection = on_connection;
	pl->data = NULL;

	return 0;
}
