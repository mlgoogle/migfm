#ifndef _CHAT_DIC_COMM_H__
#define _CHAT_DIC_COMM_H__
#include "chat_basic_infos.h"
#include "base/logic_comm.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <libmemcached/memcached.h>

namespace chat_storage{

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static bool Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
	static bool CheckToken(const int64 platform_id,int64 user_id,
		const char* current_token);
	static bool GetUserInfo(const int64 platform_id,int64 user_id,
		                    chat_base::UserInfo& userinfo);
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

	static bool SetUserPushConfig(int64 uid, const std::string &device_token,
			int is_receive, unsigned begin_time, unsigned end_time);

	static bool GetUserPushConfig(int64 uid, std::string &device_token,
			bool &is_receive, unsigned &begin_time, unsigned &end_time);

	static bool GenaratePushMsgID(int64& msg_id);

	static bool StagePushMsg(int64 uid, int64 msg_id, const std::string& msg);
#if defined (_DIC_POOL_)
	static base_storage::DictionaryStorageEngine* RedisConnectionPop(void);
	static void RedisConnectionPush(base_storage::DictionaryStorageEngine* engine);
#endif

private:
	static std::list<base::ConnAddr>   addrlist_;
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
