#ifndef __SVR_PLUGIN_H__
#define __SVR_PLUGIN_H__
#include<stdio.h>
#include<stdlib.h>
#include"buffer.h"
#include "common.h"

#define PLUGIN_DATA_ID                char *id;
#define PLUGIN_DATA_NAME              char *name;
#define PLUGIN_DATA_VERSION           char *version;
#define PLUGIN_DATA_PROVIDER          char *provider;  


int plugins_load(struct server* srv);
void plugins_free(struct server* srv);

handler_t plugins_call_init(struct server* srv);
handler_t plugins_call_clean_up(struct server* srv,void* pd_t);
handler_t plugins_call_handler_read(struct server *srv,int fd,void *pd,int len);
handler_t plugins_call_handler_read_srv(struct server *srv,int fd,void *pd,int len);
handler_t plugins_call_connection(struct server *srv,int fd,void *pd,int len);
handler_t plugins_call_connection_close(struct server *srv,int fd);
handler_t plugins_call_connection_srv(struct server *srv,int fd,void *pd,int len);
handler_t plugins_call_connection_close_srv(struct server *srv,int fd);
handler_t plugins_call_clock_handler(struct server *srv,int fd);
handler_t plugins_call_handler_read_other(struct server *srv,int fd,void *pd,int len);
handler_t plugins_call_time_msg(struct server *srv,char* id,int opcode,int time);
handler_t plugins_call_handler_init_time(struct server *srv);
struct plugin* get_pluginst_info(struct server *srv,const char* id);
#endif
