#ifndef __MIG_CHAT_LOGIC_COMM_H__
#define __MIG_CHAT_LOGIC_COMM_H__

#include "comm_head.h"
#include "protocol.h"
#include "log/mig_log.h"
#include "base/thread_handler.h"
#include "base/thread_lock.h"
#include "basic/basictypes.h"
#include "storage/storage.h"

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

const unsigned DEFAULT_BEGIN_TIME = 8*60;
const unsigned DEFAULT_END_TIME = 23*60;

namespace logic {

class ThreadKey {
public:
	static void InitThreadKey ();
	static base_storage::DBStorageEngine *GetStorageDBConn ();
	static void SetStorageDBConn (base_storage::DBStorageEngine *conn);
	static base_storage::DictionaryStorageEngine* GetStorageDicConn ();
	static void SetStorageDicConn(base_storage::DictionaryStorageEngine *conn);
	static void DeinitThreadKey ();
	static threadkey_handler_t *db_key_;
	static threadkey_handler_t *dic_key_;
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
	struct threadrw_t *m_lock;

public:
	WLockGd (struct threadrw_t *lock)
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

class SomeUtils{
public:
//
	static char *SafeStrncpy(char *dest, size_t dest_size, const char *src,
		               size_t src_size);

	static void GetCurrntTimeFormat(std::string& current_time);

//Send Interface
	static int SendFull (int socket, const char *buffer, 
		size_t nbytes);

	static bool SendBytes (int socket, const void *bytes, int len, 
		const char *file, long line);

	static bool SendMessage (int socket, struct PacketHead *packet, 
		const char *file, long line);

	static bool SendHeadMessage (int socket, int32 operate_code, int16 msg_type,
		int8 is_zip,int session, const char *file, long line);

	static bool SendErrorCode(int socket,int32 operate_code,int16 msg_type,int8 is_zip,
			int32 session,const int32 error_code,const char *file,long line);

};

}

#define sendbytes (socket,bytes,len)     \
	logic::SomeUtils::SendBytes(socket,bytes,len,__FILE__,__LINE__)

#define sendmessage (socket,packet)      \
	logic::SomeUtils::SendMessage(socket,packet,__FILE__,__LINE__)

#define sendheadmsg(socket,operate_code,msg_type,is_zip,session)  \
	logic::SomeUtils::SendHeadMessage(socket,operate_code,msg_type,is_zip,session,__FILE__,__LINE__)

#define senderror(socket,operate_code,is_zip,session,error_code)  \
	logic::SomeUtils::SendErrorCode(socket,operate_code,ERROR_TYPE,is_zip,session,error_code,__FILE__,__LINE__)

#endif
