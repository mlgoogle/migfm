#ifndef _MASTER_PLUGIN_SSO_LOGIC_COMM_H__
#define _MASTER_PLUGIN_SSO_LOGIC_COMM_H__

#include "thread_handler.h"
#include "log/mig_log.h"
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





#define DEFAULT_CONFIG_PATH     "./plugins/identity_provider/identity_provider_config.xml"
namespace sso_logic{
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
};

}
#endif