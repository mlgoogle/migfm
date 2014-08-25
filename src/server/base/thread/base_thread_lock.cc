#include "base_thread_lock.h"

int InitThreadMutex (threadmutex_t **mtx, unsigned int flags){
    
	int ret = 0;
    threadmutex_t *_mtx;

    _mtx = (threadmutex_t *) malloc (sizeof (threadmutex_t));
    if (_mtx == NULL) {
        ret = ENOMEM;
        goto ERROR_CLEAN;
    }

    if (flags == PTHREAD_MUTEX_RECURSIVE) {
        pthread_mutexattr_t mattr;

        pthread_mutexattr_init (&mattr);
        pthread_mutexattr_settype (&mattr, PTHREAD_MUTEX_RECURSIVE);
        ret = pthread_mutex_init (&_mtx->mutex, &mattr);
        pthread_mutexattr_destroy (&mattr);
    } else
        ret = pthread_mutex_init (&_mtx->mutex, NULL);

    if (ret)
        goto ERROR_CLEAN;
    *mtx = _mtx;

    return ret;

ERROR_CLEAN:
    free (_mtx);

    return ret;
}

void DeinitThreadMutex (threadmutex_t *mtx)
{
    assert (mtx);
    free (mtx);
}

int LockThreadMutex (threadmutex_t *mtx)
{
    assert (mtx);
    return pthread_mutex_lock (&mtx->mutex);
}

int TryLockThreadMutex (threadmutex_t *mtx)
{
    assert (mtx);
    return pthread_mutex_trylock (&mtx->mutex);
}

int UnLockThreadMutex (threadmutex_t *mtx)
{
    assert (mtx);
    return pthread_mutex_unlock (&mtx->mutex);
}


int InitThreadCond (threadcond_t **cond)
{
    int ret = 0;
    threadcond_t *_cond;

    _cond = (threadcond_t *) malloc (sizeof (threadcond_t));
    if (_cond == NULL) {
        ret = ENOMEM;
        goto ERROR_CLEAN;
    }

    if (ret = pthread_cond_init (&_cond->cond, NULL))
        goto ERROR_CLEAN;
    *cond = _cond;

    return ret;

ERROR_CLEAN:
    free (_cond);

    return ret;
}

void DeinitThreadCond (threadcond_t *cond)
{
    assert (cond);
    free (cond);
}

int WaitThreadCond (threadcond_t *cond, threadmutex_t *mtx)
{
    assert (cond && mtx);
    return pthread_cond_wait (&cond->cond, &mtx->mutex);
}

int TimedWaitThreadCond (threadcond_t *cond, threadmutex_t *mtx, struct timespec *timeout)
{
    assert (cond && mtx);
    return pthread_cond_timedwait (&cond->cond, &mtx->mutex, timeout);
}

int SignalThreadCond (threadcond_t *cond)
{
    assert (cond);
    return pthread_cond_signal (&cond->cond);
}

int BroadCastThreadCond (threadcond_t *cond)
{
    assert (cond);
    return pthread_cond_broadcast (&cond->cond);
}

int InitThreadrw (threadrw_t **rwlock)
{
    int ret = 0;
    threadrw_t *_rwlock;

    _rwlock = (threadrw_t *) malloc (sizeof (threadrw_t));
    if (_rwlock == NULL) {
        ret = ENOMEM;
        goto ERROR_CLEAN;
    }

    if (ret = pthread_rwlock_init (&_rwlock->rwlock, NULL))
        goto ERROR_CLEAN;
    *rwlock = _rwlock;

    return ret;

ERROR_CLEAN:
    free (_rwlock);

    return ret;
}

void DeinitThreadrw (threadrw_t *rwlock)
{
    assert (rwlock);
    free (rwlock);
}

int RlockThreadrw (threadrw_t *rwlock)
{
    assert (rwlock);
    return pthread_rwlock_rdlock (&rwlock->rwlock);
}

int TryrLockThreadrw (threadrw_t *rwlock)
{
    assert (rwlock);
    return pthread_rwlock_tryrdlock (&rwlock->rwlock);
}

int WlockThreadrw (threadrw_t *rwlock)
{
    assert (rwlock);
    return pthread_rwlock_wrlock (&rwlock->rwlock);
}

int TrywLockThreadrw (threadrw_t *rwlock)
{
    assert (rwlock);
    return pthread_rwlock_trywrlock (&rwlock->rwlock);
}

int UnlockThreadrw (threadrw_t *rwlock)
{
    assert (rwlock);
    return pthread_rwlock_unlock (&rwlock->rwlock);
}
