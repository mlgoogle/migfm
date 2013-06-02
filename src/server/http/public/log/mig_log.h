#ifndef _MIG_LOG_H_
#define _MIG_LOG_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


enum {
    NO_PRIORITY,
    DEBUG_PRIORITY,
    INFO_PRIORITY,
    NOTICE_PRIORITY,
    WARN_PRIORITY,
    ERROR_PRIORITY
};

enum {
    USER_LEVEL,
    SYSTEM_LEVEL
};


#define MIG_DEBUG(level, format, ...)  \
        log_impl(__FILE__, __LINE__, __FUNCTION__, (level), DEBUG_PRIORITY, (format), ##__VA_ARGS__)

#define MIG_INFO(level, format, ...)   \
		log_impl(__FILE__, __LINE__, __FUNCTION__, (level), INFO_PRIORITY, (format), ##__VA_ARGS__)

#define MIG_NOTICE(level, format, ...) \
		log_impl(__FILE__, __LINE__, __FUNCTION__, (level), NOTICE_PRIORITY, (format), ##__VA_ARGS__)

#define MIG_WARN(level, format, ...)   \
		log_impl(__FILE__, __LINE__, __FUNCTION__, (level), WARN_PRIORITY, (format), ##__VA_ARGS__)

#define MIG_ERROR(level, format, ...)  \
        log_impl(__FILE__, __LINE__, __FUNCTION__, (level), ERROR_PRIORITY, (format), ##__VA_ARGS__)

#define MIG_LOG(level, format, ...)    \
        log_impl(NULL, 0, NULL, (level), INFO_PRIORITY, (format), ##__VA_ARGS__)

#define MIG_LOGSMP(level, format, ...) \
        log_impl(NULL, 0, NULL, (level), NO_PRIORITY, (format), ##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

int log_init ();

int log_impl (const char *file, const int line, const char *func, int level, int priority, const char *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif // _MIG_LOG_H_
