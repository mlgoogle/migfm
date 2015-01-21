#include"network.h"
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<event.h>
#include<malloc.h>
#include<stddef.h>
#include<linux/types.h>
#include<errno.h>
#include<netinet/in.h>
#include<sys/un.h>
#define NDEBUG
#include<assert.h>
#include<unistd.h>
#include<arpa/inet.h>
#include "common.h"
#include "network.h"
#include "plugins.h"
#include "buffer.h"
#include "log/mig_log.h"

#define SOCK_ADAPTER_HASHSIZE (64*1024)

static int packet_count = 0;
static int echo_conn_count = 0;
static int sock_conn_count = 0;
static int adapter_count = 0;
static int block_count = 0;
static struct sock_adapter* conn_head = NULL;
enum PACKET_STATE{
    STATE_NEW = 0,
    STATE_READ_HEADER,
    STATE_READ_PAY_LOAD
};

static inline struct list_head *sock_adapter_sock2hash(struct server *srv,int sock){

    return &(srv->sock_adapter_hash[sock&(SOCK_ADAPTER_HASHSIZE-1)]);
}

struct psock_conn *alloc_sock_conn(struct server *srv){
    //sock_conn_count++;
    __sync_fetch_and_add(&sock_conn_count,1);
    return ((struct psock_conn*)malloc(sizeof(struct psock_conn)));
}

void free_sock_conn(struct psock_conn* conn,struct server *srv){
    //sock_conn_count--;
    __sync_fetch_and_sub(&sock_conn_count,1);
    free(conn);
    conn=NULL;
    //srv->sock_conn_count = sock_conn_count;
}

static inline void sock_conn_addref(struct psock_conn *conn) {
   conn->psc_refcount++;
}

struct sock_adapter *alloc_sock_adapter(struct server *srv){
    struct sock_adapter *sa;
    //adapter_count++;
    __sync_fetch_and_add(&adapter_count,1);
    sa = (struct sock_adapter *)malloc(sizeof(*sa));
    return sa;
}

void free_sock_adapter(struct sock_adapter *sa,
                       struct server *srv)
{
    //adapter_count--;
    __sync_fetch_and_sub(&adapter_count,1);
    free(sa);
    sa = NULL;
}

int create_sock_adapter_table(struct server* srv){

    struct list_head  *hash;

    int i;

    hash = (struct list_head*)malloc(SOCK_ADAPTER_HASHSIZE*sizeof(*hash));

    if(hash==NULL){
        MIG_ERROR(USER_LEVEL,"can't alloc sock adapter hash table");
        return -1;
    }
   
    for(i = 0;i<SOCK_ADAPTER_HASHSIZE;i++)
       INIT_LIST_HEAD(&hash[i]);

    srv->sock_adapter_hash = hash;
   
    return 0;
}

void destroy_sock_adapter_table(struct server *srv)
{

    int i;
    if(srv->sock_adapter_hash==NULL)
        return;
    for(i=0;i<SOCK_ADAPTER_HASHSIZE;i++)
        assert(list_empty(&srv->sock_adapter_hash[i]));

    free(srv->sock_adapter_hash);

    srv->sock_adapter_hash = NULL;
}


struct sock_adapter *find_sock_adapter(int sock,struct server *srv){
    unsigned int index = sock&(SOCK_ADAPTER_HASHSIZE-1);
    struct list_head *sas = &srv->sock_adapter_hash[index];
    struct list_head *tmp;
    struct sock_adapter *sa;

    list_for_each(tmp,sas){
        sa = list_entry(tmp,struct sock_adapter,list);
        if(sa->sock == sock)
            return sa;
    }

    return NULL;
}

void destroy_sock_adapter(struct sock_adapter *sa,struct server *srv){

    //Originate from sock_conn
    event_del(&sa->ev);
    list_del(&sa->list);
    
    if(sa->sock!=-1){
        close(sa->sock);
        sa->sock=-1;
    }
	if(sa->type==CONNECT)
    	buffer_free(sa->ip);
    free_sock_adapter(sa,srv);
}

static inline struct echo_block *echo_block_alloc(struct echo_conn *conn,
                         struct server *srv){

    struct echo_block *block;
    char *addr = (char*)malloc(6);
    if(addr==NULL)
        return NULL;

    block = (struct echo_block *)malloc(sizeof(*block));
    if(block==NULL){
        free(addr);
        addr=NULL;
        return NULL;
    }
    
    assert(block_count>=0);
    __sync_fetch_and_add(&block_count,1);
    block->address = addr;
    block->length = 6;
    block->refcount = 0;
    
    return block;
}

static inline void echo_block_free(struct echo_block *block,struct server *srv){

    assert(block->refcount==0);
    __sync_fetch_and_sub(&block_count,1);
    free(block->address);
    free(block);
}

static inline struct echo_packet *echo_packet_alloc(struct echo_conn *conn,
                                                    struct server *srv){
    struct echo_packet *packet;
    __sync_fetch_and_add(&packet_count,1);
    packet = (struct echo_packet*)malloc(sizeof(*packet));
    memset(packet,'\0',sizeof(*packet));
    packet->conn = conn;
    return packet;
}

static inline void echo_packet_free(struct echo_packet *packet,
                                    struct server *srv){
    __sync_fetch_and_sub(&packet_count,1);
    free(packet);
    packet=NULL;
}

static inline void echo_block_addref(struct echo_block *block){
    __sync_fetch_and_add(&block->refcount,1);
}

static inline void echo_block_decref(struct echo_block *block,
                                     struct server *srv){
    if(!(__sync_sub_and_fetch(&block->refcount,1)))
        echo_block_free(block,srv);
}

static inline struct echo_conn *echo_conn_alloc(struct psock_conn *sc,struct server *srv){

    struct echo_conn *ec;
    ec = (struct echo_conn*)malloc(sizeof(*ec));
    if(ec==NULL)
        return NULL;
    memset(ec,'\0',sizeof(*ec));
    __sync_fetch_and_add(&echo_conn_count,1);
    INIT_LIST_HEAD(&ec->pending_packets);
    INIT_LIST_HEAD(&ec->inflight_packets);
    ec->privates = sc;
    ec->srv = srv;
    pthread_mutex_init(&ec->lock,0);
    return ec;
}

static inline void echo_conn_free(struct echo_conn *conn,struct server *srv){
    __sync_fetch_and_sub(&echo_conn_count,1); 
    assert(list_empty(&conn->pending_packets));
    assert(list_empty(&conn->inflight_packets));
    if(conn->block){
        echo_block_decref(conn->block,srv);
    }
    if(conn->packet){
        echo_packet_free(conn->packet,srv);
    }
    pthread_mutex_destroy(&conn->lock);
    free(conn);
    conn=NULL;
}


static void block_add_tail(struct echo_packet *packet, struct echo_block *block,
                           int offset, int length) {
  struct echo_block *first;
  if (packet->block_count == 0) {
    packet->u.one_block.one_block = block;
    packet->u.one_block.offset = offset;
    packet->u.one_block.length = length;
  }
  else if (packet->block_count == 1) {
    first = packet->u.one_block.one_block;
    int first_offset = packet->u.one_block.offset;

    INIT_LIST_HEAD(&packet->u.blocks.blocks);
    list_add_tail(&first->list, &packet->u.blocks.blocks);
    packet->u.blocks.head_position = first_offset;
    list_add_tail(&block->list, &packet->u.blocks.blocks);
    packet->u.blocks.tail_offset = offset + length;
  }
  else {
    list_add_tail(&block->list, &packet->u.blocks.blocks);
    packet->u.blocks.tail_offset = offset + length;
  }
  __sync_fetch_and_add(&packet->block_count,1);
  echo_block_addref(block);
}


int echo_post_recv(struct echo_conn *conn,int status,struct server *srv){

    struct psock_conn *sc = (struct psock_conn*)conn->privates;
    if(conn->block==NULL){
        conn->block = echo_block_alloc(conn,srv);
        if(conn->block==NULL){
            return -ENOMEM;
        }
        echo_block_addref(conn->block);
    }

	sc->psc_rx_size = conn->block->length - conn->offset;
    sc->psc_rx_buffer = conn->block->address + conn->offset;
    return status;
}

void output_packet(struct echo_packet *packet,struct server *srv) {
    struct psock_conn *conn = (struct psock_conn*)packet->conn->privates;
    struct echo_block *block;
    //int rc;
    /*while (1) {
    rc = send(conn->psc_sock, "\n", 1, 0);
    printf("rc = %d, errno=%d\n", rc, errno);
    if (rc >= 0)
      break;
    else {
      if (errno != EAGAIN)
        break;
    }
  }*/
  
    if (packet->block_count == 1) {
        //printf("%*.*s\n", packet->u.one_block.length-4, packet->u.one_block.length-4,
          //     packet->u.one_block.one_block->address+4+packet->u.one_block.offset);
        plugins_call_handler_read(srv,conn->psc_sock,
                                  packet->u.one_block.one_block->address+packet->u.one_block.offset,
                                  packet->u.one_block.length);
        echo_block_decref(packet->u.one_block.one_block,srv);
    }
    else {
        int i;
        struct buffer *buf;
        buf = buffer_init_string("");
        for (i = 0; i < packet->block_count; i++) {
            block = list_entry(packet->u.blocks.blocks.next, struct echo_block, list);
            list_del(&block->list);
            if (i == 0) {
                int len = block->length-packet->u.blocks.head_position;
                buffer_append_string_len(buf,block->address+packet->u.blocks.head_position,len);
                 //printf("%*.*s\n", len-4, len-4,
                 //block->address+4+packet->u.blocks.head_position);
            }
            else if (i != packet->block_count-1) {
                //printf("%*.*s\n", block->length, block->length, block->address);
                buffer_append_string_len(buf,block->address,block->length);
            }
            else {
                  //printf("%*.*s\n", packet->u.blocks.tail_offset,
                   //      packet->u.blocks.tail_offset, block->address);
                  buffer_append_string_len(buf,block->address,packet->u.blocks.tail_offset);
            }
            echo_block_decref(block,srv);
      }
            //printf("%s\n",buf->ptr+4);
            plugins_call_handler_read(srv,conn->psc_sock,buf->ptr,buf->used);
            buffer_free(buf);
    }
}

void echo_handle_packet(struct echo_packet *packet,struct server *srv) {
    
    struct packet_buffer* pack_buff;
    if(packet->block_count==1){
    	pack_buff = (struct packet_buffer*)malloc(sizeof(struct packet_buffer));
    	if(pack_buff==NULL)
            return;
        pack_buff->buf = buffer_init_string("");
		//MIG_DEBUG(USER_LEVEL,"%*.*s\n", packet->u.one_block.length, packet->u.one_block.length,
		//	packet->u.one_block.one_block->address + packet->u.one_block.offset);
        buffer_append_string_len(pack_buff->buf,
		    packet->u.one_block.one_block->address+packet->u.one_block.offset,
			packet->u.one_block.length);
        pack_buff->type = packet->type;
        pack_buff->sock = packet->sock;
        pack_buff->srv = srv;
		echo_block_decref(packet->u.one_block.one_block,srv);
    }else{
        int i;
        pack_buff = (struct packet_buffer*)malloc(sizeof(struct packet_buffer));
        if(pack_buff==NULL)
            return;
        pack_buff->buf = buffer_init_string("");
        for(i=0;i<packet->block_count;i++){
            struct echo_block* block = list_entry(packet->u.blocks.blocks.next,
			                           struct echo_block,list);
            if(i==0){
                int len = block->length-packet->u.blocks.head_position;
                buffer_append_string_len(pack_buff->buf,
						                 block->address+packet->u.blocks.head_position,
										 len); 
				//MIG_DEBUG(USER_LEVEL,"%*.*s\n", len, len,
				//	block->address+packet->u.blocks.head_position);
            }else if(i!=packet->block_count-1){
                buffer_append_string_len(pack_buff->buf,block->address,block->length);
				//MIG_DEBUG(USER_LEVEL,"%*.*s\n", block->length, block->length, block->address);
            }else{
                buffer_append_string_len(pack_buff->buf,block->address,packet->u.blocks.tail_offset);
				//MIG_DEBUG(USER_LEVEL,"%*.*s\n", packet->u.blocks.tail_offset,
				//	packet->u.blocks.tail_offset, block->address);
            }
            __list_del(block->list.prev,block->list.next);
            pack_buff->type = packet->type;
            pack_buff->sock = packet->sock;
            pack_buff->srv = srv;
			echo_block_decref(block,srv);
        }
    }

	//MIG_INFO(USER_LEVEL,"[%d] %s",pack_buff->buf->used-1,pack_buff->buf->ptr);
    srv->system_addtask(srv,PACKET,pack_buff);
    list_del(&packet->list);
    echo_packet_free(packet,srv);
}

int echo_forward(void *privates,int length,struct server *srv)
{
    struct echo_conn *conn = (struct echo_conn*)privates;
    struct echo_block *block = conn->block;
    char *ptr =block->address + conn->offset;
    char *p = ptr;
    char *tail = ptr + length;
    int len,status = 0;
    struct psock_conn *spconn = (struct psock_conn*)conn->privates;
    struct echo_packet *packet = conn->packet;
    conn->offset += length;
    assert(conn->offset<=block->length);

	while(1) {
		while (p < tail && *p != '\n') {
			if (*p == 0)
				printf("recv an error char: 0\n");
			p++;
		}

		if (*p != '\n' && conn->offset < block->length) {
			/* do nothing */
			break;
		}
      else {
      struct echo_packet *packet = conn->packet;
      if (packet == NULL) {
        packet = echo_packet_alloc(conn,srv);
        if (packet == NULL) {
          return -ENOMEM;
        }
        conn->packet = packet;
      }

			// a whole block
			if (*p== '\n'){
			     len = ++p - block->address - conn->prev_packet_offset;
			     ptr = p;

			     block_add_tail(packet, block, conn->prev_packet_offset, len);
			     conn->prev_packet_offset += len;

			     list_add_tail(&packet->list, &conn->inflight_packets);
			     conn->packet = NULL;
				 packet->srv=srv;
				 packet->type = ((struct sock_adapter *)(spconn->psc_adapter))->type;
				 packet->sock = ((struct sock_adapter *)(spconn->psc_adapter))->sock;
			     echo_handle_packet(packet,srv);
			}else{
				assert(conn->offset == block->length);

				status = -EAGAIN;

				len = block->length - conn->prev_packet_offset;
				block_add_tail(packet, block, conn->prev_packet_offset, len);

				conn->prev_packet_offset = 0;
				conn->offset = 0;
				echo_block_decref(block,srv);
				conn->block = NULL;
				break;
			}
		}

	}

    return echo_post_recv(conn,status,srv);
}

struct psock_conn *create_sock_conn(struct server *srv){

    struct psock_conn *conn;
    conn = alloc_sock_conn(srv);
    if(conn==NULL){
        MIG_ERROR(USER_LEVEL,"alloc sock_conn failed\n");
        return NULL;
    } 

    memset(conn,'\0',sizeof(*conn));
    conn->psc_sock = -1;
    conn->psc_refcount = 1;
   
    INIT_LIST_HEAD(&conn->psc_rx_list);
    INIT_LIST_HEAD(&conn->psc_tx_list);
    INIT_LIST_HEAD(&conn->psc_tx_queue);
    return conn;
}

void destroy_sock_conn(struct psock_conn *conn,struct server *srv){
    printf("destroy_sock_conn\n");
    assert(list_empty(&conn->psc_rx_list));
    assert(list_empty(&conn->psc_tx_list));
    assert(list_empty(&conn->psc_tx_queue));
    struct sock_adapter* sa = (struct sock_adapter*)conn->psc_adapter;
    if(sa->type==CONNECT)
    	sa->port = 0;
    	//memset(srv->connect_pool[sa->index],'\0',sizeof(struct sock_adapter));
    echo_conn_free((struct echo_conn*)conn->psc_cookie,srv);
    destroy_sock_adapter((struct sock_adapter*)conn->psc_adapter,srv);
    free_sock_conn(conn,srv);
}

static inline void sock_conn_decref(struct psock_conn* conn,struct server *srv) {
    struct sock_adapter* sa = (struct sock_adapter*)conn->psc_adapter;
    if (!(--conn->psc_refcount)){
	    if(sa->type==0)
            plugins_call_connection_close(srv,conn->psc_sock);
	    else
	        plugins_call_connection_close_srv(srv,conn->psc_sock);
		
		destroy_sock_conn(conn,srv);
  }
}

void sockbase_schedule_lock(struct psock_sched *sched){

}

void sockbase_schedule_unlock(struct psock_sched *sched){

}

int sockbase_receive(struct psock_conn *conn){
    int rc,err=0;
    for(;;){
        rc = recv(conn->psc_sock,conn->psc_rx_buffer,conn->psc_rx_size,0);
//    		if (rc>0)
//    			MIG_DEBUG(USER_LEVEL,"conn->psc_rx_size[%d] [%s] rc[%d]",conn->psc_rx_size,
//    			      conn->psc_rx_buffer,rc);
        if(rc>0){
            conn->psc_rx_started = 1;
            conn->psc_rx_deadline = time(NULL)+5;
            err =rc;
        }
        else if(rc<=0){
            if(rc<0)
                err = -errno;
            else
                err=0;
            if(rc==0&&conn->psc_rx_started){
                err=-EPROTO;
            }
            break;
        }
        break;
    }
    return err;
}


int sockbase_process_receive(struct psock_conn *conn,struct server *srv){
    int rc;
again:

    if(conn->psc_rx_nob_wanted!=0){
        rc = sockbase_receive(conn);
        if(rc<=0){
            if(rc==-EAGAIN)
                return -EAGAIN;
            if(rc==0){
                //read eof
                sock_conn_decref(conn,srv);
            }
            else if(!conn->psc_closing){
                //read error
                sock_conn_decref(conn,srv);
            }

			if(rc!=-EAGAIN){
                sock_conn_decref(conn,srv);
			}
           
            return rc==0?-ESHUTDOWN:rc;
        }
        assert(conn->psc_rx_nob_wanted==-1);
      
        rc = echo_forward(conn->psc_cookie,rc,srv);
        if(rc==-EAGAIN){
            goto again;
        }
    }
    
    return rc;
}

int sockbase_process_transmit(struct psock_conn *conn, struct psock_tx *tx) {
    return 0;
}

void destroy_tx(struct psock_tx *tx) {

}


int sockbase_schedule(struct psock_sched *sched,struct server *srv){
    int rc = 0;
    struct psock_conn *conn;
    struct psock_tx *tx;
    int did_something = 0;

    sockbase_schedule_lock(sched);
    
    while(1){
        did_something = 0;
        if(!list_empty(&sched->pss_rx_conns)){
            conn = list_entry(sched->pss_rx_conns.next,
                              struct psock_conn,psc_rx_list);
            list_del_init(&conn->psc_rx_list);
            
            assert(conn->psc_rx_scheduled);
            assert(conn->psc_rx_ready);
          
            conn->psc_rx_ready=0;
            
            sockbase_schedule_unlock(sched);
            rc = sockbase_process_receive(conn,srv);
            sockbase_schedule_lock(sched);
            assert(conn->psc_rx_scheduled);
            if (rc == 0)
                conn->psc_rx_ready = 1;

            if (conn->psc_rx_ready) {
                list_add_tail(&conn->psc_rx_list, &sched->pss_rx_conns);
            }
            else {
                conn->psc_rx_scheduled = 0;
                sock_conn_decref(conn,srv);
            }
                did_something = 1;
        }
        if (!list_empty(&sched->pss_tx_conns)) {
            conn = list_entry(sched->pss_tx_conns.next,
                              struct psock_conn,psc_tx_list);
            list_del(&conn->psc_tx_list);
            assert(conn->psc_tx_scheduled);
            assert(conn->psc_tx_ready);
            assert(!list_empty(&conn->psc_tx_queue));

            tx = list_entry(conn->psc_tx_queue.next, struct psock_tx, tx_list);
            list_del(&tx->tx_list);

            conn->psc_tx_ready = 0;
            sockbase_schedule_unlock(sched);

            rc = sockbase_process_transmit(conn, tx);

            if (rc == -ENOMEM || rc == -EAGAIN) {
                sockbase_schedule_lock(sched);
                list_add(&tx->tx_list, &conn->psc_tx_queue);
            }
            else {
                destroy_tx(tx);

                sockbase_schedule_lock(sched);
                conn->psc_tx_ready = 1;
            }

            if (rc == -ENOMEM) {
                assert(0);
            }
            else if (conn->psc_tx_ready &&
                     !list_empty(&conn->psc_tx_queue)) {
                list_add_tail(&conn->psc_tx_list, &sched->pss_tx_conns);
            }
            else {
                conn->psc_tx_scheduled = 0;
            }
            did_something = 1;
        }

        if (!did_something) {
            break;
        }
    }

    sockbase_schedule_unlock(sched);
    return rc;
}

void server_read_buffer(int fd,short which,void *arg){
    struct sock_adapter *sa = (struct sock_adapter*)arg;
    struct psock_conn *conn = sa->conn;
    struct server *srv = ((struct echo_conn*)conn->psc_cookie)->srv;
    assert(fd==sa->sock);
    assert(which!=EV_TIMEOUT);
   
    if(which==EV_SIGNAL)
        return;
    
    assert(which==EV_READ);
    
    sockbase_schedule_lock(conn->psc_scheduler);
    conn->psc_rx_ready = 1;
   
    if(!conn->psc_rx_scheduled){
        list_add_tail(&conn->psc_rx_list,&conn->psc_scheduler->pss_rx_conns);
        conn->psc_rx_scheduled = 1;
        sock_conn_addref(conn);
    }
   
   sockbase_schedule_unlock(conn->psc_scheduler);
   
   sockbase_schedule(conn->psc_scheduler,srv);
}

void server_write_buffer(int fd, short which, void *arg) {
  struct sock_adapter *sa = (struct sock_adapter *)arg;
  struct psock_conn *conn = sa->conn;
  assert(fd == sa->sock);
  assert(which != EV_TIMEOUT);
  if (which == EV_SIGNAL)
    return;

  assert(which == EV_WRITE);

  sockbase_schedule_lock(conn->psc_scheduler);

  conn->psc_tx_ready = 1;

  if (!conn->psc_tx_scheduled && // not being progressed
      !list_empty(&conn->psc_tx_queue)) { // packets to send
    list_add_tail(&conn->psc_tx_list, &conn->psc_scheduler->pss_tx_conns);
    conn->psc_tx_scheduled = 1;
    sock_conn_addref(conn);
  }

  sockbase_schedule_unlock(conn->psc_scheduler);
}

////////////////////////////////////////////////////////////////////////////

//临时解决方案 //外连socket 单独创建 。但不接受数据处理，除非数据格式和监听socket 一样

static int create_connect_socket(){
    int sock;
    struct sockaddr_in sai;
    int rc,opt;
    sock =socket(AF_INET,SOCK_STREAM,0);

	if(sock<0){
        MIG_ERROR(USER_LEVEL,"unable to create server socket[%d]]\n",errno);
        return 0;
    }
    opt = 1;
    rc = setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt));
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"unable to set SO_KEEPALIVE on server socket%d\n",errno);
        close(sock);
        return 0;
    }
    rc = setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"unable to set SO_REUSEADDR on servert socket:%d\n",errno);
        return 0;
    }
    return sock;
}

static int  create_socket(){ // 0 net_work  1,process_work
    int sock;
    struct sockaddr_in sai;
    int rc,opt;

#if defined (NET_WORK)
    sock =socket(AF_INET,SOCK_STREAM,0);
#endif

#if defined (PROCESS_WORK)
	sock =socket(AF_UNIX,SOCK_STREAM,0);
#endif
	if(sock<0){
        MIG_ERROR(USER_LEVEL,"unable to create server socket[%d]]\n",errno);
        return 0;
    }
    opt = 1;
    rc = setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt));
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"unable to set SO_KEEPALIVE on server socket%d\n",errno);
        close(sock);
        return 0;
    }
    rc = setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"unable to set SO_REUSEADDR on servert socket:%d\n",errno);
        return 0;
    }
    return sock;
}

struct sock_adapter* create_connect_socket(struct server* srv,
                                           const char* host,
                                           const char* port){
    struct sock_adapter* conn = NULL;
    struct sockaddr_in sai;
    int rc,opt;
    int sock;
    int flags;
    struct psock_conn* sc = NULL;
    assert(atoi(port)>0&&atoi(port)<65536);
    conn = alloc_sock_adapter(srv);
    
    if(conn==NULL){
        MIG_ERROR(USER_LEVEL,"alloc sock adapter failed");
        return NULL;
    }
    conn->type = CONNECT;
    
    conn->port = atol(port);
    
    conn->ip = buffer_init_string(host);
    
    sock = create_connect_socket();
    opt = 1;
    sai.sin_family = AF_INET;
    sai.sin_port = htons(atoi(port));
    sai.sin_addr.s_addr = inet_addr(host);
    sc = create_sock_conn(srv);
    if(sc==NULL){
        MIG_ERROR(USER_LEVEL,"fail to allocate sock_conn\n");
        free_sock_adapter(conn,srv);
        close(sock);
        return NULL;
    }

    rc = connect(sock,(const struct sockaddr *)&sai,sizeof(sai));
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"connect error (%d)  (%s)\n",errno,strerror(errno));
        free_sock_adapter(conn,srv);
        close(sock);
        return NULL;
    }
    
    plugins_call_connection_srv(srv,sock,(void*)host,atoi(port));

    sc->psc_sock = sock;
    sc->psc_scheduler = &srv->one_scheduler;
    sc->psc_adapter = conn;
    sc->psc_rx_nob_left = sc->psc_rx_nob_wanted =-1;
    sc->psc_cookie = (void*)echo_conn_alloc(sc,srv);
    
    if(sc->psc_cookie==NULL){
        MIG_ERROR(USER_LEVEL,"failed to allocate echo_conn\n");
	      free_sock_adapter(conn,srv);
	      close(sock);
	      return NULL;
    } 
    sc->psc_rx_state = STATE_NEW;
    rc = echo_post_recv((struct echo_conn*)sc->psc_cookie,0,srv);
    if(sc->psc_cookie==NULL){
        MIG_ERROR(USER_LEVEL,"failed to allocate echo_conn\n");
	      free_sock_adapter(conn,srv);
	      close(sock);
	      return NULL;
    }

    sc->psc_myipaddr =0;
    sc->psc_ipaddr = NULL;
    sc->psc_port = 0;
    conn->conn = sc;
    conn->sock = sock;
    if(conn_head==NULL){
        conn->parent = NULL;
	      conn_head = conn;
    } 
    else{
        conn->parent = conn_head;
    }
    conn->flags = EV_READ;

    event_set(&conn->ev,sock,EV_READ|EV_PERSIST,server_read_buffer,conn);
    rc = event_add(&conn->ev,NULL);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"event_add failed (%d)\n",errno);
        free_sock_adapter(conn,srv);
        close(sock);
        return NULL;
    }
    list_add_tail(&conn->list,sock_adapter_sock2hash(srv,sock));
    //connect server

    flags = fcntl(sock,F_GETFL,0);
    rc = fcntl(sock,F_SETFL,flags|O_NONBLOCK);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,
                   "fcntl set socket non-block failed:%d\n",
                   errno);
        free_sock_adapter(conn,srv);
        close(sock);
        return NULL;
    }
    //connect plugin
    return conn; 
}

#if defined (NET_WORK)
struct sock_adapter *create_listen_socket(struct server* srv,int port){
#endif
#if defined (PROCESS_WORK)
struct sock_adapter *create_listen_socket(struct server* srv,const char* path){
#endif

    struct sock_adapter* sa = NULL;
#if defined (NET_WORK)
    struct sockaddr_in sai;
#endif

#if defined (PROCESS_WORK)
    struct sockaddr_un sai;
#endif

    int rc,opt;
    int sock;
	struct stat tstat;
#if defined (NET_WORK)
    assert(port>0&&port<65536);
#endif
    sa = alloc_sock_adapter(srv);
    if(sa==NULL){
        MIG_ERROR(USER_LEVEL,"alloc sock adapter failed");
        return NULL;
    }
    INIT_LIST_HEAD(&sa->list);
    sa->sock = -1;
    sa->conn = NULL;
    sa->flags = 0;
    sa->parent = NULL;
    sa->type = 0;
    sock = create_socket();
    opt = 1;
#if defined (NET_WORK)
    sai.sin_family = AF_INET;
    sai.sin_port = htons(port);
    sai.sin_addr.s_addr = 0;
#endif

#if defined (PROCESS_WORK)
	if(lstat(path,&tstat)==0){
		if(S_ISSOCK(tstat.st_mode))
			unlink(path);
	}
	sai.sun_family = AF_UNIX;
	memset(sai.sun_path,'\0',sizeof(sai.sun_path));
	strncpy(sai.sun_path,path,strlen(path));
#endif

    rc = bind(sock,(const struct sockaddr *)&sai,sizeof(sai));
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"unable bind socket:%s\n",strerror(errno));
        close(sock);
        free_sock_adapter(sa,srv);
        return NULL;
    }

    rc = listen(sock,0);
    if(rc<0){
        MIG_ERROR(USER_LEVEL,"unable to listen to server socket :%d\n",errno);
        close(sock);
        free_sock_adapter(sa,srv);
        return NULL;
    }

	int err = chmod(path,0777);
	if (err<0){
	    MIG_ERROR(USER_LEVEL,"=====chmod error %s=========",strerror(errno));
	}
    //set socket no-block
    opt = fcntl(sock,F_GETFL,0);
    rc = fcntl(sock,F_SETFL,opt|O_NONBLOCK);
    if(rc!=0){
    
        MIG_ERROR(USER_LEVEL,"fcntl set sock non-block failed:%d\n",errno);
        close(sock);
        free_sock_adapter(sa,srv);
        return NULL;
    }

    sa->sock = sock;
    list_add_tail(&sa->list,sock_adapter_sock2hash(srv,sock));
    return sa;
}

void server_accept(int fd,short which,void *arg){
    struct sockaddr_in addr;
    int sock;
    socklen_t addrlen;
    int flags,rc;
    struct server *srv = (struct server*)arg;
    struct sock_adapter *conn;
    struct sock_adapter *sa = srv->sa;
    struct psock_conn *sc;
    int err;
	int r;
    assert(fd==sa->sock);
    addrlen = sizeof(addr);
    sock = accept(fd,(struct sockaddr *)&addr,&addrlen);
    if(sock<0){
		MIG_ERROR(USER_LEVEL,"accept error [%s]",strerror(errno));
        err =errno;
        if(err==EWOULDBLOCK) 
            return;
        else
            return;    
    }
    flags = fcntl(sock,F_GETFL,0);
    rc = fcntl(sock,F_SETFL,flags|O_NONBLOCK);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"fcntl set socket non-block failed:%d\n",errno);
        close(sock);
        return;
    }
    conn = alloc_sock_adapter(srv);
    if(conn==NULL){
        MIG_ERROR(USER_LEVEL,"alloc sock adapter failed\n");
        close(sock);
        return;
    }
    conn->flags = 0;
    conn->type = ACCEPT;
    sc = create_sock_conn(srv);
    if(sc==NULL){
        
        MIG_ERROR(USER_LEVEL,"fail to allocate sock_conn\n");
        free_sock_adapter(conn,srv);
        close(sock);
        return;
    }

    sc->psc_sock = sock;
    sc->psc_scheduler = &srv->one_scheduler;
    sc->psc_adapter = conn;
	sc->psc_rx_nob_left = sc->psc_rx_nob_wanted = -1;
    sc->psc_cookie = (void*)echo_conn_alloc(sc,srv);

    if(sc->psc_cookie==NULL){
        MIG_ERROR(USER_LEVEL,"failed to allocate eho_conn\n");
        destroy_sock_conn(conn->conn,srv);
        free_sock_adapter(conn,srv);
        close(sock);
        return; 
    }
    sc->psc_rx_state = STATE_NEW; 
    rc = echo_post_recv((struct echo_conn*)sc->psc_cookie,0,srv);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"failed to post recv:%d\n",rc);
        destroy_sock_conn(conn->conn,srv);
        free_sock_adapter(conn,srv);
        close(sock);
        return;
    }

    sc->psc_myipaddr = 0;
    sc->psc_ipaddr = ntohl(addr.sin_addr.s_addr);
    sc->psc_port = ntohs(addr.sin_port);
   
    conn->conn = sc;

    
    conn->sock = sock;
    conn->parent =sa;
    conn->flags = EV_READ;
    
    r = plugins_call_connection(srv,sock,NULL,0);
   	if(r!=HANDLER_GO_ON){
    	close(sock);
    	return;
	}
    //srv->tmp_sa = conn; 
    event_set(&conn->ev,sock,EV_READ|EV_PERSIST,server_read_buffer,conn);
    rc = event_add(&conn->ev,NULL);

    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"event_add failed (%d)\n",errno);
        destroy_sock_conn(conn->conn,srv);
        free_sock_adapter(conn,srv);
        close(sock);
        return;
    }

    list_add_tail(&conn->list,sock_adapter_sock2hash(srv,sock));
    return;
}

void time_cb(int fd, short event, void *argc) {
  struct server *srv = (struct server*)argc;
  event_base_loopbreak(srv->base);
}


int network_init(struct server *srv){

   int rc;

   rc = create_sock_adapter_table(srv);
   
   if(rc==-1)
        return rc;

   memset(&srv->one_scheduler,'\0',sizeof(srv->one_scheduler));

   INIT_LIST_HEAD(&srv->one_scheduler.pss_rx_conns);

   INIT_LIST_HEAD(&srv->one_scheduler.pss_tx_conns);

   srv->one_scheduler.pss_nconns = 0;
   
   srv->register_event = register_event;
   
   srv->create_reconnects = create_reconnects;

   return rc;
}


int free_network(struct server *srv)
{
    destroy_sock_adapter(srv->sa,srv);
    event_base_free(srv->base);
    destroy_sock_adapter_table(srv);
}

int network_register_fdevents(struct server *srv)
{
   
    int rc; 
    srv->base = event_init();

    if(srv->base==NULL){
        MIG_ERROR(USER_LEVEL,"event_init failed\n");
        return -1;
    }
#if defined (NET_WORK)
    srv->sa = create_listen_socket(srv,atoi(srv->srv_conf.port->ptr));
#endif

#if defined (PROCESS_WORK)
	srv->sa = create_listen_socket(srv,srv->srv_conf.process_path->ptr);
#endif

    if(srv->sa==NULL){
        return -1;
    }

    srv->sa->flags = EV_READ|EV_PERSIST;

    event_set(&srv->sa->ev,srv->sa->sock,EV_READ|EV_PERSIST,server_accept,srv);
   
    rc = event_add(&srv->sa->ev,NULL);
    
    if(rc!=0){
        free_network(srv);
    }
    return 0;
}

int network_start(struct server *srv)
{
   int rc;
   rc  =  event_base_dispatch(srv->base);
   return rc;
}

int network_stop(struct server *srv)
{
   destroy_sock_adapter(srv->sa,srv);  
   
   event_base_free(srv->base);
  
   destroy_sock_adapter_table(srv);
   
   if(srv->connect_pool){free(srv->connect_pool);srv->connect_pool;}
}

int create_reconnects(struct server *srv){
    struct list_head* tmp;
    struct connect_config* p;
    struct sock_adapter* sa = NULL;
    struct sock_adapter* tmp_sa = NULL;
    int n =srv->ncount_connect;
    int index = 0;
    if(srv==NULL)
    	return 0;
    if(list_empty(&srv->srv_conf.remotes)){
    	 MIG_ERROR(USER_LEVEL,"srv->srv_conf.remotes empty");
         return 0;
    }
    if(srv->connect_pool==NULL){
    	 MIG_ERROR(USER_LEVEL,"srv->connect_pool empty");
         return -1;
    }
    list_for_each(tmp,&srv->srv_conf.remotes){
    	p = list_entry(tmp,struct connect_config,list);
    	tmp_sa = srv->connect_pool[index];
    	if((tmp_sa->port!=atol(p->port->ptr))){
    		sa = create_connect_socket(srv,p->bindremote->ptr,p->port->ptr);
                if(sa==NULL){
                    tmp_sa->port = 0;
                    return -1;
                }
    		srv->connect_pool[index] = sa;
    		sa->index = index;
    		index++;
    	}else{
		    MIG_ERROR(USER_LEVEL,"connect error sa port[%d] srv_conf.remotes port[%d]",
					    tmp_sa->port,atol(p->port->ptr));
		}
    }
    
    return 1;
}


int create_connects(struct server *srv){
    int n = 0;
    struct list_head* tmp;
    struct connect_config* p;
    struct sock_adapter* sa = NULL;
    if(list_empty(&srv->srv_conf.remotes))
        return 0;
    srv->connect_pool = (struct sock_adapter**)calloc(srv->ncount_connect,sizeof(struct sock_adapter*));
    if(srv->connect_pool==NULL)
        return -1;
    list_for_each(tmp,&srv->srv_conf.remotes){
	p = list_entry(tmp,struct connect_config,list);
        sa = create_connect_socket(srv,p->bindremote->ptr,p->port->ptr);
		if(sa!=NULL){
			srv->connect_pool[n] = sa;
			sa->index = n;
			n++;
		}
    }
    //assert(n==srv->ncount_connect);
    return 0;
}

int register_event(struct server *srv,int fd,short events){
	struct psock_conn* sc = NULL;
	struct sock_adapter* conn = NULL;
	int rc;
	int flags;
	if(srv==NULL)
		return 0;
	conn = alloc_sock_adapter(srv);
	if(conn==NULL){
	    MIG_ERROR(USER_LEVEL,"alloc sock adapter failed");
		return 1 ;
	}
	conn->type = OTHER;
	sc = create_sock_conn(srv);
	if(sc==NULL){
		MIG_ERROR(USER_LEVEL,"fail to allocate sock_conn\n");
		free_sock_adapter(conn,srv);
		return 0;
	}

	sc->psc_sock = fd;
	sc->psc_scheduler = &srv->one_scheduler;
	sc->psc_adapter = conn;
	sc->psc_cookie = (void*)echo_conn_alloc(sc,srv);

	if(sc->psc_cookie==NULL){
		MIG_ERROR(USER_LEVEL,"failed to allocate echo_conn\n");
		free_sock_adapter(conn,srv);
		return 0;
	}

	sc->psc_rx_state = STATE_NEW;
	rc = echo_post_recv((struct echo_conn*)sc->psc_cookie,0,srv);

    if(sc->psc_cookie==NULL){
        MIG_ERROR(USER_LEVEL,"failed to allocate echo_conn\n");
	    free_sock_adapter(conn,srv);
	    return 0;
    }

	sc->psc_myipaddr = 0;
	sc->psc_ipaddr = NULL;
	conn->conn = sc;
	conn->sock = fd;

	if(conn_head==NULL){
		conn->parent = NULL;
		conn_head =conn;
	}
	else{
		conn->parent =conn_head;
	}

	conn->flags = EV_READ;
    if(srv->base==NULL){
        MIG_ERROR(USER_LEVEL,"base init error(%d)\n",errno);
        free_sock_adapter(conn,srv);
        return 0;
    }
    event_set(&conn->ev,fd,EV_READ|EV_PERSIST,server_read_buffer,conn);
    rc = event_add(&conn->ev,NULL);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"event_add failed (%d)\n",errno);
        free_sock_adapter(conn,srv);
        return 0;
    }
    list_add_tail(&conn->list,sock_adapter_sock2hash(srv,fd));
    
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,
                   "fcntl set socket non-block failed:%d\n",
                   errno);
        free_sock_adapter(conn,srv);
        return NULL;
    }
    return 1; 

}
