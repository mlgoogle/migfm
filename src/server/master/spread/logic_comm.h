#ifndef _MASTER_PLUGIN_SPREAD_LOGIC_COMM__H__
#define _MASTER_PLUGIN_SPREAD_LOGIC_COMM__H__

#include "thread_handler.h"
#include "thread_lock.h"
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "storage/storage.h"
#include <string>
#include <sstream>


#define DEFAULT_CONFIG_PATH     "./plugins/spread/spread_config.xml"
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

namespace spread_logic{

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

class SomeUtils{
public:

	static void GetUTF8(std::string &msg,std::string &out_str,
							 int32 flag = 1);

	static int SendFull (int socket, const char *buffer, size_t nbytes);

	static void GetCurrentTimeFormat(const time_t current,std::string& current_time);

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

class HttpComm {
public:
	static bool PushMessage(const std::string &device_token, const std::string &msg,
			int badge=1, const std::string &sound="");

	class WeiXin{
	public:
		static bool GetWeiXinToken(const std::string& appid,const std::string& secret,
								std::string& content);

		static bool PostWeiXinMenu(const std::string& access_token,const std::string& content,
				std::string& result);
	};
};

class ResolveJson{
public:
	class WeiXin{
	public:
		static bool ResolveWeiXinGetToken(const std::string& content,std::string& token,
								         int32 exp_time);

		static bool ResolveWeiXinGetMenuState(const std::string& content);
	};
};

}
#endif
