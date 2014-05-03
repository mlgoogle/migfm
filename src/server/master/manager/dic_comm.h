#ifndef _MASTER_PLUGIN_CHAT_MGR_DIC_COMM__H__
#define _MASTER_PLUGIN_CHAT_MGR_DIC_COMM__H__

#include "thread_handler.h"
#include "thread_lock.h"
#include "logic_comm.h"
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include <list>

namespace manager_storage{

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

private:
	static base_storage::DictionaryStorageEngine* engine_;
};


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
#if defined (_DIC_POOL_)
	static base_storage::DictionaryStorageEngine* RedisConnectionPop(void);
	static void RedisConnectionPush(base_storage::DictionaryStorageEngine* engine);
#endif
public:
	static bool GetUserPushConfig(int64 uid, std::string &device_token,
				bool &is_receive, unsigned &begin_time, unsigned &end_time);

	static bool StagePushMsg(int64 uid, int64 msg_id, const std::string& msg);

	static bool GenaratePushMsgID(int64& msg_id);
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


}

#endif
