#ifndef _MASTER_COMMON_H__
#define _MASTER_COMMON_H__

#include "buffer.h"
#include "linuxlist.h"
#include "event.h"
#include "base_threadpool.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<linux/types.h>
#include<aio.h>

typedef enum handler_t{
	HANDLER_UNSET,
	HANDLER_GO_ON,
	HANDLER_FINISHED,
	HANDLER_COMEBACK,
	HANDLER_WAIT_FOR_EVENT,
	HANDLER_ERROR,
	HANDLER_WAIT_FOR_FD
}handler_t;

typedef enum srv_state{
	SERVER_NO_START=0,
	SERVER_INIT_SRV,
	SERVER_RD_CONF,
	SERVER_INIT_LOG,
	SERVER_RD_PLUGINS,
	SERVER_LOAD_PLUGINS,
	SERVER_INIT_PLUGINS,
	SERVER_INIT_TP,
	SERVER_INIT_AIO,
	SERVER_INIT_NET,
	SERVER_INIT_CLOCK,
	SERVER_REG_EVENT,
	SERVER_START_NET 
}srv_state;

typedef enum type_task {
	PACKET=0,
	CLOCK=1
}type_task;

typedef enum connect_type{
	ACCEPT = 0,
	CONNECT = 1,
	OTHER = 2
}connect_type;

struct psock_sched{

	struct list_head pss_rx_conns;

	struct list_head pss_tx_conns;

	struct list_head pss_reap_conns;

	pthread_cond_t pss_waitq;

	int pss_nconns;

	pthread_mutex_t pss_lock;
};

struct thread_adapter{

	struct server *srv;

	void* data;
};


struct psock_conn{

	int psc_sock;

	struct list_head psc_list;

	int psc_refcount;

	struct psock_sched *psc_scheduler;

	struct list_head psc_reap_list;

	int psc_myipaddr;

	__u32 psc_ipaddr;

	__u32   psc_port;

	void  *psc_adapter;

	int  psc_closing;


	/*reader*/

	struct list_head psc_rx_list;

	time_t psc_rx_deadline;

	__u8 psc_rx_started;

	__u8 psc_rx_ready;

	__u8 psc_rx_scheduled;

	__u8 psc_rx_state;

	int  psc_rx_nob_left;

	int  psc_rx_nob_wanted;

	int  psc_rx_size;

	char *psc_rx_buffer;


	void *psc_cookie;


	/*writer*/

	struct list_head psc_tx_list;

	struct list_head psc_tx_queue;

	time_t psc_tx_deadline;

	int  psc_tx_nob;

	int  psc_tx_ready;

	int  psc_tx_scheduled;
};


struct psock_tx{

	struct list_head tx_list;

	int tx_refcount;

	int tx_offset;

	void *tx_packet;

	struct psock_conn *tx_conn;
};


struct plugin_desc{

	struct buffer  *id;

	struct buffer  *name;

	struct buffer  *version;

	struct buffer  *provider;

	struct buffer  *path;

	struct list_head list;

};

struct sock_adapter{

	struct list_head list;

	int sock;

	short flags;

	int   type; //0,accpet 1,connect

	int   connectd;

	int   index;

	struct event ev;

	struct buffer* ip;

	int    port;

	struct psock_conn *conn;

	struct sock_adapter *parent;

};

struct connect_config{
	struct buffer   *port;
	struct buffer   *bindremote;
	struct list_head list;
};

struct plugin_xml_path{

	struct buffer  *filepath;

	struct list_head list;

	struct stat stat;
};


struct server_config{

	struct buffer   *bindhost;

	struct buffer   *error_file;

	struct buffer   *usr_file;

	struct buffer   *sys_file;

	struct buffer   *username;

	struct buffer   *modules_dir;

	struct buffer   *max_worker;

	struct buffer   *max_fds;

	struct buffer   *max_conns;
#if defined (NET_WORK)
	struct buffer   *port;
#endif

#if defined (PROCESS_WORK)
    struct buffer   *process_path;
#endif

	struct list_head  remotes;
};

struct time_task{
	struct list_head list;
	char* id;
	int opcode;
	int time;
	int current_time;
	int ncount;
	struct server* srv;
};

struct netaio {
	struct list_head  link;
	struct aiocb cb;
};

struct plugin {

	struct buffer    *id;

	struct buffer    *name;

	struct buffer    *provider;

	struct buffer    *version;

	void*     lib;

	void*     data;    

	void*     (*init)();

	handler_t (*clean_up)(struct server* svr,void* pd_t);

	handler_t (*handler_init_time)(struct server* srv);

	handler_t (*thread_func)(struct server *srv,int fd,void* data);

	handler_t (*handler_read)(struct server *srv,int fd,void* data,int len);

	handler_t (*handler_read_srv)(struct server *srv,int fd,void* data,int len);

	handler_t (*handler_read_other)(struct server *srv, int fd,void* data,int len);

	handler_t (*connection)(struct server *srv,int fd,void* data,int len);

	handler_t (*connection_close)(struct server *srv,int fd);

	handler_t (*init_time)(struct server *srv,int fd,void* data,int len);

	handler_t (*connection_srv)(struct server *srv,int fd,void* data,int len);

	handler_t (*connection_close_srv)(struct server* srv,int fd);

	handler_t (*time_msg)(struct server* srv,char* id,int opcode,int time);
};


struct socket_adapter {
	struct list_head  link;
	int sock;
};

struct server{

	struct server_config     srv_conf;

	struct list_head         plugins_xml_path;

	struct list_head         plugins_des;

	struct buffer            *module_path;

	struct buffer            *tmp_buf;

	struct buffer            plugins;

	void                     *plugins_slot;

	struct list_head         *sock_adapter_hash;

	struct psock_sched       one_scheduler;

	struct event_base        *base;

	struct sock_adapter      *sa;

	base_threadpool_t        *thrp;

	int                      state;

	int                      ncount_connect;

	struct sock_adapter**    connect_pool;

	int (*user_addtask)(struct server *srv,int fd,struct plugin *pl);

	int (*system_addtask)(struct server *srv,int fd,void *data);

	int (*time_addtask)(struct server* srv,void* data);

	int (*register_event)(struct server* srv,int fd,short events);

	int (*add_time_task)(struct server* srv,char* id,int opcode,
		                 int time,int ncount);

	void (*del_time_task)(struct server* srv,char* id,int opcode);

	int  (*create_reconnects)(struct server *srv);
};
#endif
