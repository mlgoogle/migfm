#ifndef __BASE_THREADLOCK_H__
#define __BASE_THREADLOCK_H__

#include "base_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef struct base_threadmutex_t	base_threadmutex_t;

    int base_init_threadmutex (base_threadmutex_t **mtx, unsigned int flags);

    void base_deinit_threadmutex (base_threadmutex_t *mtx);

    int base_lock_threadmutex (base_threadmutex_t *mtx);

    int base_trylock_threadmutex (base_threadmutex_t *mtx);

    int base_unlock_threadmutex (base_threadmutex_t *mtx);



    typedef struct base_threadcond_t	base_threadcond_t;

    int base_init_threadcond (base_threadcond_t **cond);

    void base_deinit_threadcond (base_threadcond_t *cond);

    int base_wait_threadcond (base_threadcond_t *cond, base_threadmutex_t *mtx);

    int base_timedwait_threadcond (base_threadcond_t *cond, base_threadmutex_t *mtx, struct timespec *timeout);

    int base_signal_threadcond (base_threadcond_t *cond);

    int base_broadcast_threadcond (base_threadcond_t *cond);




    typedef struct base_threadrw_t	base_threadrw_t;

    int base_init_threadrw (base_threadrw_t **rwlock);

    void base_deinit_threadrw (base_threadrw_t *rwlock);

    int base_rlock_threadrw (base_threadrw_t *rwlock);

    int base_tryrlock_threadrw (base_threadrw_t *rwlock);

    int base_wlock_threadrw (base_threadrw_t *rwlock);

    int base_trywlock_threadrw (base_threadrw_t *rwlock);

    int base_unlock_threadrw (base_threadrw_t *rwlock);


#ifdef __cplusplus
}
#endif

#endif /* __BASE_THREADLOCK_H__ */
