#ifndef __BASE_CONFIG_H__
#define __BASE_CONFIG_H__

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

#endif /* __BASE_CONFIG_H__ */
