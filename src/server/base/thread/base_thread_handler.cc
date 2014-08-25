#include "base_thread_handler.h"

int InitThreadAttr(threadattr_t **thrdattr)
{
    *thrdattr = (threadattr_t*)malloc (sizeof (threadattr_t));
    if (*thrdattr == NULL)
        return ENOMEM;

    return 0;
}

int ThreadAttrSetDetach(threadattr_t *thrdattr, int on)
{
    int ret;

    ret = pthread_attr_setdetachstate(&thrdattr->attr, on ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);

    return ret;
}

int ThreadAttrSetStacksize(threadattr_t *thrdattr, size_t size)
{
    int ret;

    ret = pthread_attr_setstacksize(&thrdattr->attr, size);

    return ret;
}

int ThreadattrSetGuardsize(threadattr_t *thrdattr, size_t size)
{
    int ret;

    ret = pthread_attr_setguardsize(&thrdattr->attr, size);

    return ret;
}

void DeinitThreadAttr(threadattr_t *thrdattr)
{
    free(thrdattr);
}

static void *fake_run (void *t)
{
    thread_handler_t *thrd = (thread_handler_t*) t;
    return thrd->thread_run_func (thrd, thrd->args);
}

int InitThread (thread_handler_t **thrd, threadattr_t *thrdattr, thread_run_t thread_run_func, void *args)
{
    int ret = 0;
    thread_handler_t *_thrd;

    _thrd = (thread_handler_t *) malloc (sizeof (thread_handler_t));
    if (_thrd == NULL) {
        ret = ENOMEM;
        goto ERROR_CLEAN;
    }

    _thrd->td = (pthread_t *) malloc (sizeof (pthread_t));
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

void DeinitThread (thread_handler_t *thrd)
{
    BASE_ASSERT (thrd);
    free (thrd->td);
    free (thrd);
}

pthread_t GetThreadid ()
{
    return pthread_self ();
}

int ExitThread (thread_handler_t *thrd, int retval)
{
    BASE_ASSERT (thrd);
    thrd->retval = retval;
    pthread_exit (NULL);
    return 0;
}

int JoinThread (thread_handler_t *thrd, int *retval)
{
    int ret = 0;
    int *thread_ret;

    BASE_ASSERT (thrd);
    if (ret = pthread_join (*thrd->td, (void **)&thread_ret))
        return ret;

    *retval = thrd->retval;

    return ret;
}

int DetachThread (thread_handler_t *thrd)
{
    int ret = 0;

    BASE_ASSERT (thrd);
    if (ret = pthread_detach (*thrd->td))
        return ret;

    return ret;
}

int ThreadOnce (thread_once_t thread_once)
{
    static const pthread_once_t once_static = PTHREAD_ONCE_INIT;

    return pthread_once (const_cast<pthread_once_t*>(&once_static), thread_once);
}

void BaseSleep(microsecond_t t)
{
    struct timeval tv;
    tv.tv_usec = t % _USEC_PER_SEC;
    tv.tv_sec = t / _USEC_PER_SEC;
    select(0, NULL, NULL, NULL, &tv);
}


int ThreadkeyInit (threadkey_handler_t **key, void (*dest)(void *))
{
    (*key) = (threadkey_handler_t *) calloc (1, sizeof(threadkey_handler_t));

    if ((*key) == NULL)
        return ENOMEM;

    return pthread_key_create (&(*key)->k, dest);
}

void *ThreadkeyGet (threadkey_handler_t *key)
{
    return pthread_getspecific (key->k);
}

int ThreadkeySet (void *val, threadkey_handler_t *key)
{
    return pthread_setspecific (key->k, val);
}

int ThreadkeyDeinit (threadkey_handler_t *key)
{
    int r;

    r = pthread_key_delete (key->k);
    free (key);

    return r;
}

