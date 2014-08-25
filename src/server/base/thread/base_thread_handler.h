#ifndef __MIGFM_BASE_THREAD_THREAD_HANDLER_H__
#define __MIGFM_BASE_THREAD_THREAD_HANDLER_H__

#include <sys/types.h>
#include <inttypes.h>
#include <errno.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <pthread.h>
#include <sys/time.h>

#define BASE_ASSERT(cond)   assert (cond)

typedef int64_t microsecond_t;

#define _USEC_PER_SEC   1000000


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct threadattr_t {
    pthread_attr_t attr;
}threadattr_t;
	
int InitThreadAttr(threadattr_t **thrdattr);

int ThreadAttrSetDetach(threadattr_t *thrdattr, int on);

int ThreadAttrSetStacksize(threadattr_t *thrdattr, size_t size);

int ThreadattrSetGuardsize(threadattr_t *thrdattr, size_t size);

void DeinitThreadAttr(threadattr_t *thrdattr);

struct thread_handler_t ;

typedef void *(*thread_run_t)(struct thread_handler_t*, void*);

typedef void (*thread_once_t)(void);

typedef struct thread_handler_t {
    pthread_t *td;
    void *args;
    thread_run_t thread_run_func;
    int retval;
}thread_handler_t;

int InitThread (thread_handler_t **thrd, threadattr_t *thrdattr, thread_run_t thread_run_func, void *args);

void DeinitThread (thread_handler_t *thrd);

pthread_t GetThreadid ();

int ExitThread (thread_handler_t *thrd, int retval);

int JoinThread (thread_handler_t *thrd, int *retval);

int DetachThread (thread_handler_t *thrd);

int ThreadOnce (thread_once_t thread_once);

void BaseSleep(microsecond_t t);

typedef struct threadkey_handler_t {
    pthread_key_t k;
}threadkey_handler_t;


int ThreadkeyInit (threadkey_handler_t **key, void (*dest)(void *));

void *ThreadkeyGet (threadkey_handler_t *key);

int ThreadkeySet (void *val, threadkey_handler_t *key);

int ThreadkeyDeinit (threadkey_handler_t *key);

#ifdef __cplusplus
}
#endif

#endif /* __BASE_THREAD_H__ */

