#include "thread.h"
#include "plugins.h"
#include "network.h"
#include "common.h"
#include "log/mig_log.h"

static int systun_packet_count_num = 0;
static int add_packet_count_num = 0;

static void *usr_trun(base_thread_t *thd,int fd,void* data)
{
    handler_t rc;
    struct server *srv  = ((struct thread_adapter*)data)->srv;
    struct plugin *pl =(struct plugin*)((struct thread_adapter*)data)->data; 
    if(strlen(pl->id->ptr)==0){
        MIG_ERROR(USER_LEVEL,"plugin's id error");
        return NULL;
    }
    if(pl->thread_func==NULL){
        MIG_ERROR(USER_LEVEL,"[%s]:plugin not thread func",pl->id->ptr);
        return NULL;
    }
 
    rc = pl->thread_func(srv,fd,pl->data); 
}

static void *time_trun(base_thread_t* thd,int flag,void *data){
	struct time_task* task = (struct time_task*)data;
	//plugin
	//SINA_LOG(SYSTEM_LEVEL,"opcode[%d] time[%d] current_time[%d]",
	    //     task->opcode,task->time,task->current_time);
	plugins_call_time_msg(task->srv,task->id,task->opcode,task->time);
	if(task->id) {free(task->id);task->id = NULL;}
	if(task){free(task);task=NULL;}
}

static void *sys_trun(base_thread_t *thd,int flag,void *data)
{
    handler_t rc;
    struct packet_buffer *packet;
    struct server* srv;
    switch(flag){
        case PACKET:
        	{
            packet = (struct packet_buffer*)data;
			//MIG_INFO(USER_LEVEL,"[%d] %s",packet->buf->used-1,
			//	      packet->buf->ptr);
            //SINA_LOG(SYSTEM_LEVEL,"sock[%d] type[%d] len[%d]",packet->sock,packet->type,packet->buf->used-1);
            if(packet->type==ACCEPT)
                plugins_call_handler_read(packet->srv,packet->sock,
                                          packet->buf->ptr,
                                          packet->buf->used-1);
            else if(packet->type==CONNECT)
                plugins_call_handler_read_srv(packet->srv,packet->sock,
                                              packet->buf->ptr,
                                              packet->buf->used-1);
			else if(packet->type==OTHER)
				plugins_call_handler_read_other(packet->srv,packet->sock,
												packet->buf->ptr,
												packet->buf->used-1);
            if(packet){
                if(packet->buf)
                    buffer_free(packet->buf);
                free(packet);
                packet = NULL;
            }
        }
        break;
        default:
            MIG_ERROR(USER_LEVEL,"flag unkown");
    }
     
}

static int user_addtask(struct server *srv,int fd,struct plugin *pl)
{
    int ret;
    if(srv==NULL||pl==NULL)
    	return 0;
    struct thread_adapter *thr_srv = (struct thread_adapter*)malloc(sizeof(*thr_srv));
    thr_srv->srv = srv;
    thr_srv->data = pl;
    ret = base_threadpool_addtask(srv->thrp,usr_trun,fd,thr_srv,0,NULL);
    return ret;
}

static int sys_addtask(struct server *srv,int fd,void *data)
{
    int ret = 0;
	if (data==NULL||srv==NULL){
		MIG_ERROR(USER_LEVEL,"data null==========");
		return 0;
	}
// 	if ((*((char*)(data)))=='\0'){
// 		SINA_ERROR(SYSTEM_LEVEL,"(char*)(data)) null==========");
// 		return 0;
// 	}
    ret = base_threadpool_addtask(srv->thrp,sys_trun,fd,data,0,NULL);

    return ret;
}

static int time_addtask(struct server* srv,void* data){
	int ret;
	if(srv==NULL||data==NULL)
		return 0;
	struct time_task* task = (struct time_task*)data;
	struct time_task* new_task = (struct time_task*)malloc(sizeof(struct time_task));
	new_task->id = (char*)malloc(strlen(task->id)+1);
	strcpy(new_task->id,task->id);
	new_task->opcode = task->opcode;
	new_task->time = task->time;
	new_task->ncount = task->ncount;
	new_task->current_time = task->current_time;
	new_task->srv = srv;
	ret = base_threadpool_addtask(srv->thrp,time_trun,0,new_task,0,NULL);
	return ret;
}

int init_threadpool(struct server *srv)
{
    int ret;
   
    ret = base_init_threadpool(&srv->thrp,0,
                               atoi(srv->srv_conf.max_worker->ptr),0);
   
    srv->user_addtask = user_addtask;
   
    srv->system_addtask = sys_addtask;

    srv->time_addtask = time_addtask;

    return ret;    
}

int stop_threadpool(struct server *srv)
{
    int ret;
    ret = base_threadpool_cancel_tasks(srv->thrp,NULL);
    ret = base_deinit_threadpool(srv->thrp);
    return ret;

}
