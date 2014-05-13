#ifndef _MASTER_PLUGIN_USR_MGR_LOGIC_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_LOGIC_COMM_H__

#include "thread_handler.h"
#include "thread_lock.h"
#include "log/mig_log.h"
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include <vector>

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

#define USERINFOS "migfm_user_infos"

#define DEFAULT_CONFIG_PATH     "./plugins/sociality_manager/sociality_manager_config.xml"

#define DEFAULT_URL "http://42.121.14.108/wx/lp5.mp3"

const unsigned DEFAULT_BEGIN_TIME = 8*60;
const unsigned DEFAULT_END_TIME = 23*60;


namespace mig_sociality {



enum RECORDTYPE{
	PARENT_TYPE = 1,
	SAYHELLO_TYPE = 2,
	MESSAGE_TYPE = 3
};


struct MusicFriendInfo{
	base::UserInfo userinfo;
	double latitude;
	double longitude;
	double distance;
};

struct Detail{
	double distance;
	int32 message_type;
	int64 uid;
	int64 fromuid;
	int64 msg_id;
	std::string cur_music;
	std::string msg_time;
	std::string message;
};

struct MessageListInfo{
	struct Detail   detail;
	base::UserInfo  userinfo;
	base::MusicInfo musicinfo;
	base::MusicInfo current_musicinfo;
};


struct RecordMessage{
	double distance;
	std::string songid;
	std::string message;
};

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
    static int SendFull (int socket, const char *buffer, size_t nbytes);

	static void GetResultMsg(std::string &status, std::string &msg,
		                     std::string &result,std::string &out_str,
							 int32 flag = 1);
	
	static void GetCurrntTimeFormat(std::string& current_time);

	static int SplitStringChr(const char *str, const char *char_set,
					   std::vector<std::string> &out);
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

} // mig_sociality
#endif
