#ifndef _NETWORK_H__
#define _NETWORK_H__
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"linuxlist.h"
struct server;

struct packet_buffer{
	
    struct buffer *buf;
    
    int sock;
    
    int type;

    struct server* srv;
};

struct echo_block{
    struct list_head list;

    char *address;

    int length;

    int refcount;
};

struct echo_conn;
struct echo_packet{

    struct list_head list;
    
    union{
        
        struct{
            struct list_head blocks;
            int head_position;
            int tail_offset;
        }blocks;

        struct{
            struct echo_block *one_block;
            int  offset;
            int  length;
        }one_block;
    }u; 
    int  block_count;
    int  dirty;
    struct server* srv;
    int sock;
    int type;
    struct echo_conn *conn;
};


struct echo_conn{

    struct list_head list;
   
    struct echo_block *block;
  
    int prev_packet_offset;

    int offset;

    struct buffer* ip;
    
    int port;
    
    int index;
        
    struct echo_packet *packet;

    struct list_head pending_packets;

    struct list_head inflight_packets;

    void *privates;

    struct server *srv; 

    pthread_mutex_t lock;
};

int network_init(struct server *srv);

int network_register_fdevents(struct server *srv);

int network_start(struct server *srv);

int create_connects(struct server *srv);

int network_stop(struct server *srv);

int register_event(struct server *srv,int fd,short events);

int create_reconnects(struct server *srv);

#endif
