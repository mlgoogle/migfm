#include "base_thread.h"

struct base_threadattr_t {
    pthread_attr_t attr;
};

int base_init_threadattr(base_threadattr_t **thrdattr)
{
    *thrdattr = malloc (sizeof (base_threadattr_t));
    if (*thrdattr == NULL)
        return ENOMEM;

    return 0;
}

int base_threadattr_set_detach(base_threadattr_t *thrdattr, int on)
{
    int ret;

    ret = pthread_attr_setdetachstate(&thrdattr->attr, on ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);

    return ret;
}

int base_threadattr_set_stacksize(base_threadattr_t *thrdattr, size_t size)
{
    int ret;

    ret = pthread_attr_setstacksize(&thrdattr->attr, size);

    return ret;
}

int base_threadattr_set_guardsize(base_threadattr_t *thrdattr, size_t size)
{
    int ret;

    ret = pthread_attr_setguardsize(&thrdattr->attr, size);

    return ret;
}

void base_deinit_threadattr(base_threadattr_t *thrdattr)
{
    free(thrdattr);
}

struct base_thread_t {
    pthread_t *td;
    void *args;
    base_thread_run_t thread_run_func;
    int retval;
};

static void *fake_run (void *t)
{
    base_thread_t *thrd = (base_thread_t*) t;
    return thrd->thread_run_func (thrd, thrd->args);
}

int base_init_thread (base_thread_t **thrd, base_threadattr_t *thrdattr, base_thread_run_t thread_run_func, void *args)
{
    int ret = 0;
    base_thread_t *_thrd;

    _thrd = (base_thread_t *) malloc (sizeof (base_thread_t));
    if (_thrd == NULL) {
        ret = ENOMEM;
        goto ERROR_CLEAN;
    }

    _thrd->td = (base_thread_t *) malloc (sizeof (base_thread_t));
    if (_thrd->td == NULL) {
        ret = ENOMEM;
        goto ERROR_CLEAN;
    }

    _thrd->args = args;
    _thrd->thread_run_func = thread_run_func;
    _thrd->retval = 0;

    if (thrdattr) {
        if (ret = pthread_create (_thrd->td, &thrdattr->attr, fake_run, _thrd))
            goto ERROR_CLEAN;
    } else {
        if (ret = pthread_create (_thrd->td, NULL, fake_run, _thrd))
            goto ERROR_CLEAN;
    }
    *thrd = _thrd;

    return ret;

ERROR_CLEAN:
    free (_thrd->td);
    free (_thrd);

    return ret;
}

void base_deinit_thread (base_thread_t *thrd)
{
    BASE_ASSERT (thrd);
    free (thrd->td);
    free (thrd);
}

pthread_t base_get_threadid ()
{
    return pthread_self ();
}

int base_exit_thread (base_thread_t *thrd, int retval)
{
    BASE_ASSERT (thrd);
    thrd->retval = retval;
    pthread_exit (NULL);
    return 0;
}

int base_join_thread (base_thread_t *thrd, int *retval)
{
    int ret = 0;
    int *thread_ret;

    BASE_ASSERT (thrd);
    if (ret = pthread_join (*thrd->td, (void *)&thread_ret))
        return ret;

    *retval = thrd->retval;

    return ret;
}

int base_detach_thread (base_thread_t *thrd)
{
    int ret = 0;

    BASE_ASSERT (thrd);
    if (ret = pthread_detach (*thrd->td))
        return ret;

    return ret;
}

int base_thread_once (base_thread_once_t thread_once)
{
    static const pthread_once_t once_static = PTHREAD_ONCE_INIT;

    return pthread_once (&once_static, thread_once);
}

void base_sleep(microsecond_t t)
{
    struct timeval tv;
    tv.tv_usec = t % _USEC_PER_SEC;
    tv.tv_sec = t / _USEC_PER_SEC;
    select(0, NULL, NULL, NULL, &tv);
}

struct base_threadkey_t {
    pthread_key_t k;
};

int base_threadkey_init (base_threadkey_t **key, void (*dest)(void *))
{
    (*key) = (base_threadkey_t *) calloc (1, sizeof(base_threadkey_t));

    if ((*key) == NULL)
        return ENOMEM;

    return pthread_key_create (&(*key)->k, dest);
}

void *base_threadkey_get (base_threadkey_t *key)
{
    return pthread_getspecific (key->k);
}

int base_threadkey_set (void *val, base_threadkey_t *key)
{
    return pthread_setspecific (key->k, val);
}

int base_threadkey_deinit (base_threadkey_t *key)
{
    int r;

    r = pthread_key_delete (key->k);
    free (key);

    return r;
}
