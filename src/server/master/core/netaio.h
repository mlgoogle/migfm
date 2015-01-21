#ifndef __NETAIO_H
#define __NETAIO_H

#include <stdlib.h>
#include <aio.h>
#include "linuxlist.h"

struct server;

#define AIOCBMAX 	2048

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////// 异步 /////////////////////////////////////////
int netaio_init(struct server *srv);

// 内部分配内存并由aio返回，调用者自己free
int netaio_send (struct server *srv, struct netaio **aio, struct socket_adapter *sa, void *data, size_t len);

int netaio_return (struct server *srv, struct netaio *aio, int *len);

// 内部分配内存并由listaio返回，调用者自己free
int netaio_multicast (struct server *srv, struct list_head *listaio, int *done, int *complet_nums, struct list_head *listsa, void *data, size_t len);

///////////////////////////////////////// 同步 /////////////////////////////////////////

//int net_send (struct server *srv, struct socket_adapter *sa, void *data, size_t len);
int net_send(struct server *srv,int sock,void* data,size_t len);

// return complet nums
int net_multicast (struct server *srv, struct list_head *listsa, void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // __NETAIO_H
