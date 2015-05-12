#ifndef __BASE_LOGIC_COMM_H__
#define __BASE_LOGIC_COMM_H__
#include "log/mig_log.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"

#if defined _DEBUG || defined DEBUG
#define LOG_DEBUG(s)        \
	MIG_DEBUG (USER_LEVEL, "%s", s)
#define LOG_DEBUG2(f, ...)  \
	MIG_DEBUG (USER_LEVEL, f, __VA_ARGS__)
#else
#define LOG_DEBUG(s)
#define LOG_DEBUG2(f, ...)
#endif

#if defined USR_LOG_ERROR
#define LOG_ERROR(s)        \
	MIG_ERROR (USER_LEVEL, "%s", s)
#define LOG_ERROR2(f, ...)  \
	MIG_ERROR (USER_LEVEL, f, __VA_ARGS__)
#else
#define LOG_ERROR(s)
#define LOG_ERROR2(f, ...)
#endif

#if defined USR_LOG_MSG
#define LOG_MSG(s)          \
	MIG_LOG (USER_LEVEL, "%s", s)
#define LOG_MSG2(f, ...)    \
	MIG_LOG (USER_LEVEL, f, __VA_ARGS__)
#else
#define LOG_MSG(s)
#define LOG_MSG2(f, ...)
#endif

namespace base_logic{

class LogicComm{
public:
	static int SendFull(int socket, const char *buffer, size_t nbytes);

	static char *SafeStrncpy(char *dest, size_t dest_size, const char *src,
			               size_t src_size);
};

class RLockGd
{
private:
	threadrw_t *m_lock;

public:
	RLockGd (threadrw_t *lock)
	{
		assert (lock);
		m_lock = lock;
		RlockThreadrw (lock);
	}

	virtual ~RLockGd ()
	{
		UnlockThreadrw (m_lock);
	}
};

class WLockGd
{
private:
	threadrw_t *m_lock;

public:
	WLockGd (threadrw_t *lock)
	{
		assert (lock);
		m_lock = lock;
		WlockThreadrw (lock);
	}

	virtual ~WLockGd ()
	{
		UnlockThreadrw (m_lock);
	}
};

}
#endif
