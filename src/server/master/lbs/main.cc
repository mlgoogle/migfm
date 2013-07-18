#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"plugins.h"
#include"log/mig_log.h"
#include "lbs_logic.h"

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

	mig_lbs::LBSLogic::GetInstance()->Init();

	using namespace mig_lbs;
	LBSLogic *logic = mig_lbs::LBSLogic::GetInstance();
	std::string resp, err;
	logic->SetPOI(100001, 120.116506, 30.295979, "data100001", resp, err);
	logic->SetPOI(100002, 120.106506, 30.205979, "data100002", resp, err);
	logic->SetPOI(100003, 120.126506, 30.294979, "data100003", resp, err);
	logic->SetPOI(100004, 120.118506, 30.293979, "data100004", resp, err);
	logic->SetPOI(100005, 120.112506, 30.298979, "data100005", resp, err);

//	logic->SearchNearby(120.116506, 30.295979, 10000, "", resp, err);

//	logic->DelPOI(100003, resp, err);

    return pl_data;
}

static handler_t  on_clean_up(struct server *srv,void* pd_t)
{
	mig_lbs::LBSLogic::FreeInstance();
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
	MIG_DEBUG(USER_LEVEL, "recv data:%s", (char *)pd);


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
