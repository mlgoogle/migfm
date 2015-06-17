#ifndef __DEBUGNEW_H__
#define __DEBUGNEW_H__


//#ifdef __cplusplus
#ifdef DEBUGNEW_CPP

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

void *operator new (size_t size);
void * operator new (size_t, const char *file, int line);
void operator delete (void *p, const char *name, int line);
void *operator new [] (size_t, const char *file, int line);
void operator delete [] (void *p, const char *name, int line);

//PE_EXPORT void operator delete (void *p);
//PE_EXPORT void operator delete [] (void *p);


//#ifndef DEBUGNEW_CPP
#define tnew new(__FILE__, __LINE__)
//#endif

#endif

//#endif

#endif
