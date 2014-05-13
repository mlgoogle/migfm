#ifndef _MASTER_PLUGIN_USR_MGR_DIC_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DIC_COMM_H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include "basic/basictypes.h"
#include <list>
namespace storage{

class RedisComm{
public:
	RedisComm(){}
	virtual ~RedisComm(){}
	static base_storage::DictionaryStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,
			const int32 dic_conn_num = 10);
	static void Dest();
public:
	static bool GenaratePushMsgID(const int64 uid, int64& msg_id);
public:
#if defined (_DIC_POOL_)
	static base_storage::DictionaryStorageEngine* RedisConnectionPop(void);
	static void RedisConnectionPush(base_storage::DictionaryStorageEngine* engine);
#endif

private:
	static std::list<base::ConnAddr>  addrlist_;
#if defined (_DIC_POOL_)
	static std::list<base_storage::DictionaryStorageEngine*>  dic_conn_pool_;
	static threadrw_t*                                        dic_pool_lock_;
#endif
};

class AutoDicCommEngine{
public:
	AutoDicCommEngine();
	virtual ~AutoDicCommEngine();
	base_storage::DictionaryStorageEngine*  GetDicEngine(){
		//if(engine_){engine_->Release();}
		return engine_;
	}
private:
	base_storage::DictionaryStorageEngine*  engine_;
};

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

	static bool SetString(const char* key,const size_t key_len,
		const char* data, size_t len);

	static bool GetString(const char* key,const size_t key_len,
		char** data,size_t* len);

	static bool DelUserinfo(const std::string& uid);

private:
	static base_storage::DictionaryStorageEngine* engine_;
};

}
#endif
