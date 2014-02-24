#include "thread.h"
#include <sstream>
#include  "http_engine.h"

static void *usr_trun(base_thread_t *thd,int fd,void* data){

}

static void *time_trun(base_thread_t* thd,int flag,void *data){

}

static void *sys_trun(base_thread_t *thd,int flag,void *data){
    spider::SpiderHttpEngine* engine_ = new spider::SpiderHttpEngineImpl();
    struct GetMusicInfo* mi = (struct GetMusicInfo*)(data);
    std::stringstream url;
    url<<"http://dbfmdb.sinaapp.com/api/songs.php?sids="<<mi->count;
    engine_->RequestHttp(url.str());
    if(mi){delete mi;mi = NULL;}
    if(engine_){delete engine_;engine_=NULL;}
}

int32 user_addtask(base_threadpool_t* thrp,int fd,void *data)
{
    int ret;
    ret = base_threadpool_addtask(thrp,usr_trun,fd,data,0,NULL);
    return ret;
}

int32 sys_addtask(base_threadpool_t* thrp,int fd,void *data)
{
    int ret = 0;
	if (data==NULL)
		return 0;
    if ((*((char*)(data)))=='\0')
		return 0;
    ret = base_threadpool_addtask(thrp,sys_trun,fd,data,0,NULL);
    return ret;
}

int32 time_addtask(base_threadpool_t* thrp,void* data){
    return 0;
}

int32 init_threadpool(base_threadpool_t*  thrp,int32 num)
{
    int ret;
   
    ret = base_init_threadpool(&thrp,0,num,0);

    return ret;    
}

int32 stop_threadpool(base_threadpool_t*  thrp)
{
    int ret;
    ret = base_threadpool_cancel_tasks(thrp,NULL);
    ret = base_deinit_threadpool(thrp);
    return ret;

}
