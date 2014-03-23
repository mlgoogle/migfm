#ifndef _MASTER_PLUGIN_CHAT_MGR_LOGIC_COMM__H__
#define _MASTER_PLUGIN_CHAT_MGR_LOGIC_COMM__H__

#include "thread_handler.h"
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "storage/storage.h"
#include <string>
#include <sstream>

struct GetLeaveMessage{
	std::string platform_id;
	std::string msg_id;
	std::string uid;
	std::string oppid;
	std::string message;
	std::string  lasttime;
};

#define DEFAULT_CONFIG_PATH     "./plugins/chat_manager/chat_manager_config.xml"
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


namespace chat_logic{

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
	static bool CheckUserToken(const std::string& platform_id,
			                   const std::string& uid,const std::string& token);
	static bool GetUserToken(const std::string& platform_id,
			                  const std::string& uid,std::string& token);

	static void GetUTF8(std::string &msg,std::string &out_str,
							 int32 flag = 1);

	static int SendFull (int socket, const char *buffer, size_t nbytes);
};

}
#endif
