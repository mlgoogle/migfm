#include "base_threadlock.h"

struct base_threadmutex_t {
    pthread_mutex_t	mutex;
};

int base_init_threadmutex (base_threadmutex_t **mtx, unsigned int flags)
{
    int ret = 0;
    base_threadmutex_t *_mtx;

    _mtx = (base_threadmutex_t *) malloc (sizeof (base_threadmutex_t));
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

void base_deinit_threadmutex (base_threadmutex_t *mtx)
{
    BASE_ASSERT (mtx);
    free (mtx);
}

int base_lock_threadmutex (base_threadmutex_t *mtx)
{
    BASE_ASSERT (mtx);
    return pthread_mutex_lock (&mtx->mutex);
}

int base_trylock_threadmutex (base_threadmutex_t *mtx)
{
    BASE_ASSERT (mtx);
    return pthread_mutex_trylock (&mtx->mutex);
}

int base_unlock_threadmutex (base_threadmutex_t *mtx)
{
    BASE_ASSERT (mtx);
    return pthread_mutex_unlock (&mtx->mutex);
}

struct base_threadcond_t {
    pthread_cond_t cond;
};

int base_init_threadcond (base_threadcond_t **cond)
{
    int ret = 0;
    base_threadcond_t *_cond;

    _cond = (base_threadcond_t *) malloc (sizeof (base_threadcond_t));
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

void base_deinit_threadcond (base_threadcond_t *cond)
{
    BASE_ASSERT (cond);
    free (cond);
}

int base_wait_threadcond (base_threadcond_t *cond, base_threadmutex_t *mtx)
{
    BASE_ASSERT (cond && mtx);
    return pthread_cond_wait (&cond->cond, &mtx->mutex);
}

int base_timedwait_threadcond (base_threadcond_t *cond, base_threadmutex_t *mtx, struct timespec *timeout)
{
    BASE_ASSERT (cond && mtx);
    return pthread_cond_timedwait (&cond->cond, &mtx->mutex, timeout);
}

int base_signal_threadcond (base_threadcond_t *cond)
{
    BASE_ASSERT (cond);
    return pthread_cond_signal (&cond->cond);
}

int base_broadcast_threadcond (base_threadcond_t *cond)
{
    BASE_ASSERT (cond);
    return pthread_cond_broadcast (&cond->cond);
}


struct base_threadrw_t {
    pthread_rwlock_t rwlock;
};

int base_init_threadrw (base_threadrw_t **rwlock)
{
    int ret = 0;
    base_threadrw_t *_rwlock;

    _rwlock = (base_threadrw_t *) malloc (sizeof (base_threadrw_t));
    if (_rwlock == NULL) {
        ret = ENOMEM;
        goto ERROR_CLEAN;
    }

    if (ret = pthread_cond_init (&_rwlock->rwlock, NULL))
        goto ERROR_CLEAN;
    *rwlock = _rwlock;

    return ret;

ERROR_CLEAN:
    free (_rwlock);

    return ret;
}

void base_deinit_threadrw (base_threadrw_t *rwlock)
{
    BASE_ASSERT (rwlock);
    free (rwlock);
}

int base_rlock_threadrw (base_threadrw_t *rwlock)
{
    BASE_ASSERT (rwlock);
    return pthread_rwlock_rdlock (&rwlock->rwlock);
}

int base_tryrlock_threadrw (base_threadrw_t *rwlock)
{
    BASE_ASSERT (rwlock);
    return pthread_rwlock_tryrdlock (&rwlock->rwlock);
}

int base_wlock_threadrw (base_threadrw_t *rwlock)
{
    BASE_ASSERT (rwlock);
    return pthread_rwlock_wrlock (&rwlock->rwlock);
}

int base_trywlock_threadrw (base_threadrw_t *rwlock)
{
    BASE_ASSERT (rwlock);
    return pthread_rwlock_trywrlock (&rwlock->rwlock);
}

int base_unlock_threadrw (base_threadrw_t *rwlock)
{
    BASE_ASSERT (rwlock);
    return pthread_rwlock_unlock (&rwlock->rwlock);
}
