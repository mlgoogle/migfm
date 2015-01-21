#include "netaio.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include "common.h"
int netaio_init(struct server *srv)
{
    srv->network_write = netaio_send;
    srv->network_write_all = netaio_multicast;
    srv->network_write_return = netaio_return;
    srv->network_send = net_send;
    srv->network_multicast = net_multicast;
    return 0;
}

int netaio_send (struct server *srv, struct netaio **aio, struct socket_adapter *sa, void *data, size_t len)
{
    int rc;

    *aio = (struct netaio *) calloc (1, sizeof(struct netaio));
    if (*aio == NULL)
        return ENOMEM;

    memset (&(*aio)->cb, 0, sizeof(struct aiocb));
    (*aio)->cb.aio_fildes = sa->sock;
    (*aio)->cb.aio_buf = data;
    (*aio)->cb.aio_offset = 0;
    (*aio)->cb.aio_nbytes = len;

    rc = aio_write (&(*aio)->cb);
    if (rc < 0)
        return rc;
    
    return 0;
}

int netaio_return (struct server *srv, struct netaio *aio, int *len)
{
    int rc;

    rc = aio_error (&aio->cb);
    if (rc == -1) {
        // error
        return errno;
    } else if (rc) {
        // in progress or already canceled
        return rc;
    }

    rc = aio_return (&aio->cb);
    if (rc == -1) {
        // failed
        return -1;
    } else if (rc == 0) {
        // eof
        *len = 0;
    } else {
        *len = rc;
    }

    return 0;
}

static void individual_handler (int signum, siginfo_t *info, void *context)
{
    int *complet_nums = (int *) info->si_value.sival_ptr;
    (*complet_nums)++;
}

static void all_handler (int signum, siginfo_t *info, void *context)
{
    int *done = (int *) info->si_value.sival_ptr;
    (*done) = 1;
}

int netaio_multicast (struct server *srv, struct list_head *listaio, int *done, int *complet_nums, struct list_head *listsa, void *data, size_t len)
{
	struct list_head *node;
	struct socket_adapter *sa;
    int rc;
    struct netaio *aio;
    int i = 0;
    struct sigevent event;
    struct sigaction action;
    struct aiocb *list[AIOCBMAX];

    *complet_nums = *done = 0;
    list_for_each (node, listsa) {
        sa = list_entry (node, struct socket_adapter, link);
        aio = (struct netaio *) calloc (1, sizeof(struct netaio));
        if (aio == NULL)
            return ENOMEM;

        aio->cb.aio_fildes = sa->sock;
        aio->cb.aio_offset = 0;
        aio->cb.aio_buf = data;
        aio->cb.aio_nbytes = len;
        aio->cb.aio_lio_opcode = LIO_WRITE;
        aio->cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        aio->cb.aio_sigevent.sigev_signo = SIGRTMIN + 1;
        aio->cb.aio_sigevent.sigev_value.sival_ptr = complet_nums;
        list_add_tail (&aio->link, listaio);
        list[i] = &aio->cb;

        ++i;
        if (i == AIOCBMAX)
            break;
    }

    memset (&event, 0, sizeof(struct sigevent));
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGRTMIN + 2;
    event.sigev_value.sival_ptr = done;

    action.sa_sigaction = individual_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction (SIGRTMIN + 1, &action, NULL);

    action.sa_sigaction = all_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction (SIGRTMIN + 2, &action, NULL);

    // submit
    rc = lio_listio (LIO_NOWAIT, list, i, &event);
    if (rc < 0)
        return errno;

    return 0;
}

static ssize_t sendfull (int socket,
                         const char *buf,
                         size_t nbytes)
{
    ssize_t amt = 0;
    ssize_t total = 0;

    do {
        amt = nbytes;
        amt = send (socket, buf, amt, 0);
        buf = (char *) buf + amt;
        nbytes -= amt;
        total += amt;
    } while (amt != -1 && nbytes > 0);

    return amt == -1 ? amt : total;
}

//int net_send (struct server *srv, struct socket_adapter *sa, void *data, size_t len)
int net_send(struct server *srv,int sock,void* data,size_t len)
{
    //return (int) sendfull (sa->sock, data, len);
    return (int) sendfull(sock,(const char*)data,len);
}

int net_multicast (struct server *srv, struct list_head *listsa, void *data, size_t len)
{
    int l;
    struct list_head *node;
    struct socket_adapter *sa;
    int complets = 0;

    list_for_each (node, listsa) {
        sa = list_entry (node, struct socket_adapter, link);
        l = (int) sendfull (sa->sock, (const char*)data, len);
        if (l != -1)
            complets++;
    }

    return complets;
}
