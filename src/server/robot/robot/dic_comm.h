#ifndef _ROBOT_DIC_COMM_H__
#define _ROBOT_DIC_COMM_H__
#include "base/logic_comm.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <hiredis.h>
#include <libmemcached/memcached.h>

namespace robot_storage{

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static bool Init(std::list<base::ConnAddr>& addrlist);
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
			const int32 dic_conn_num = 5);
	static void Dest();

#if defined (_DIC_POOL_)
	static base_storage::DictionaryStorageEngine* RedisConnectionPop(void);
	static void RedisConnectionPush(base_storage::DictionaryStorageEngine* engine);
#endif

	static bool GetMusicInfos(const int64 songid,std::string& musicinfo);

	static int  GetHashSize(const std::string& key);

	static bool GetBatchMusicInfos(const std::string& type,std::list<int64>& random_list,
			                  std::list<std::string>& songinfolist);

	static bool GetBatchInfos(base_storage::DictionaryStorageEngine*engine,
	                      const std::string& command,std::list<std::string>& batchinfolist);


	static base_storage::CommandReply* _CreateReply(redisReply* reply);

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
