#include<stdio.h>
#include<stdlib.h>
#include"base_threadpool.h"
#include"base_threadlock.h"
#include"base_thread.h"
struct server;
int init_threadpool(struct server *srv);


int stop_threadpool(struct server *srv);
