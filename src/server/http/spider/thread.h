#ifndef _SPIDER_THREAD_H__
#define _SPIDER_THREAD_H__

#include <stdio.h>
#include <stdlib.h>
#include "thread/base_threadpool.h"
#include "thread/base_threadlock.h"
#include "thread/base_thread.h"
#include "basic/basictypes.h"
struct GetMusicInfo{
	int32 count;
};


int32 init_threadpool(base_threadpool_t*  thrp,int32 num);

int32 stop_threadpool(base_threadpool_t*  thrp);

int32 time_addtask(base_threadpool_t* thrp,void* data);

int32 sys_addtask(base_threadpool_t* thrp,int fd,void *data);

int32 user_addtask(base_threadpool_t* thrp,int fd,void *data);

#endif
