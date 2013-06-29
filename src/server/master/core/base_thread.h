#ifndef __BASE_THREAD_H__
#define __BASE_THREAD_H__

#include "base_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef struct base_threadattr_t base_threadattr_t;

    int base_init_threadattr(base_threadattr_t **thrdattr);

    int base_threadattr_set_detach(base_threadattr_t *thrdattr, int on);

    int base_threadattr_set_stacksize(base_threadattr_t *thrdattr, size_t size);

    int base_threadattr_set_guardsize(base_threadattr_t *thrdattr, size_t size);

    void base_deinit_threadattr(base_threadattr_t *thrdattr);

    typedef struct base_thread_t base_thread_t;

    typedef void *(*base_thread_run_t)(base_thread_t*, void*);

    typedef void (*base_thread_once_t)(void);

    int base_init_thread (base_thread_t **thrd, base_threadattr_t *thrdattr, base_thread_run_t thread_run_func, void *args);

    void base_deinit_thread (base_thread_t *thrd);

    pthread_t base_get_threadid ();

    int base_exit_thread (base_thread_t *thrd, int retval);

    int base_join_thread (base_thread_t *thrd, int *retval);

    int base_detach_thread (base_thread_t *thrd);

    int base_thread_once (base_thread_once_t thread_once);

    void base_sleep(microsecond_t t);

    typedef struct base_threadkey_t base_threadkey_t;

    int base_threadkey_init (base_threadkey_t **key, void (*dest)(void *));

    void *base_threadkey_get (base_threadkey_t *key);

    int base_threadkey_set (void *val, base_threadkey_t *key);

    int base_threadkey_deinit (base_threadkey_t *key);

#ifdef __cplusplus
}
#endif

#endif /* __BASE_THREAD_H__ */
