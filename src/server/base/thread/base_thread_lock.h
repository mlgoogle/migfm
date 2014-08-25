#ifndef ___MIGFM_BASE_THREAD_THREAD_LOCK_H__
#define ___MIGFM_BASE_THREAD_THREAD_LOCK_H__

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
 
struct threadmutex_t {
    pthread_mutex_t	mutex;
};

int InitThreadMutex (threadmutex_t **mtx, unsigned int flags);

void DeinitThreadMutex (threadmutex_t *mtx);

int LockThreadMutex (threadmutex_t *mtx);

int TryLockThreadMutex (threadmutex_t *mtx);

int UnLockThreadMutex (threadmutex_t *mtx);



struct threadcond_t {
    pthread_cond_t cond;
};

int InitThreadCond (threadcond_t **cond);

void DeinitThreadCond (threadcond_t *cond);

int WaitThreadCond (threadcond_t *cond, threadmutex_t *mtx);

int TimedWaiThreadCond (threadcond_t *cond, threadmutex_t *mtx, struct timespec *timeout);

int SignalThreadCond (threadcond_t *cond);

int BroadCastThreadCond (threadcond_t *cond);




struct threadrw_t {
    pthread_rwlock_t rwlock;
};


int InitThreadrw (threadrw_t **rwlock);

void DeinitThreadrw (threadrw_t *rwlock);

int RlockThreadrw (threadrw_t *rwlock);

int TryrLockThreadrw (threadrw_t *rwlock);

int WlockThreadrw (threadrw_t *rwlock);

int TrywLockThreadrw (threadrw_t *rwlock);

int UnlockThreadrw (threadrw_t *rwlock);

#endif
