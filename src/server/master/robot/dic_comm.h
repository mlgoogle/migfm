#ifndef _MASTER_PLUGIN_ROBOT_DIC_COMM__H__
#define _MASTER_PLUGIN_ROBOT_DIC_COMM__H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "logic_comm.h"
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include <list>
#include <string>

namespace robot_storage{

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetUserToken(const std::string& platform_id,
			const std::string& uid,std::string& token);

private:
	static base_storage::DictionaryStorageEngine* engine_;
};

class RedisComm{
public:
	RedisComm(){}
	virtual ~RedisComm(){}
	static base_storage::DictionaryStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 dic_conn_num = 10);
	static void Dest();

	static bool AddNewMusicInfo(const std::string& songid,const std::string& album,
				const std::string& title,const std::string& pubtime,const std::string& artist,
				const std::string& pic);
public:
#if defined (_DIC_POOL_)
	static base_storage::DictionaryStorageEngine* RedisConnectionPop(void);
	static void RedisConnectionPush(base_storage::DictionaryStorageEngine* engine);
#endif

#if defined (_DIC_POOL_)
	static std::list<base_storage::DictionaryStorageEngine*>  dic_conn_pool_;
	static threadrw_t*                                        dic_pool_lock_;
#endif

private:
	static std::list<base::ConnAddr>   addrlist_;
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
