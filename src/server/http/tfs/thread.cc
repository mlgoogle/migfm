#include "thread.h"
#include <sstream>
#include "log/mig_log.h"
#include "sprider_engine.h"
#include "config/config.h"

static void *usr_trun(base_thread_t *thd,int fd,void* data){
	spider::SpiderEngine* engine_ = new spider::SpiderEngine();
	config::FileConfig* config = static_cast<config::FileConfig*>(data);
	engine_->Init(config->mysql_db_list_);
 	while(1){
 		engine_->GetSprider();
		sleep(20);
	}
}

static void *time_trun(base_thread_t* thd,int flag,void *data){

}

static void *sys_trun(base_thread_t *thd,int flag,void *data){
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

int32 init_threadpool(base_threadpool_t**  thrp,int32 num)
{
    int ret;
   
    ret = base_init_threadpool(thrp,0,num,0);

    return ret;    
}

int32 stop_threadpool(base_threadpool_t*  thrp)
{
    int ret;
    ret = base_threadpool_cancel_tasks(thrp,NULL);
    ret = base_deinit_threadpool(thrp);
    return ret;

}
