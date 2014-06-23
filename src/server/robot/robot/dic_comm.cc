#include "dic_comm.h"
#include "base/logic_comm.h"
#include <sstream>

namespace robot_storage{



base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;
std::list<base::ConnAddr>  RedisComm::addrlist_;


bool MemComm::Init(std::list<base::ConnAddr>& addrlist){
	bool r = false;
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	if (engine_==NULL)
		return false;
	r = engine_->Connections(addrlist);
	return r;
}

void MemComm::Dest(){
	if (engine_){
		delete engine_;
		engine_ = NULL;
	}
}



#if defined (_DIC_POOL_)
threadrw_t* RedisComm::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisComm::dic_conn_pool_;
#endif

AutoDicCommEngine::AutoDicCommEngine()
:engine_(NULL){
#if defined (_DIC_POOL_)
	engine_ = robot_storage::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DIC_POOL_)
	robot_storage::RedisComm::RedisConnectionPush(engine_);
#endif
}


void RedisComm::Init(std::list<base::ConnAddr>& addrlist,
		const int32 dic_conn_num){
	addrlist_ = addrlist;

#if defined (_DIC_POOL_)
	bool r =false;
	InitThreadrw(&dic_pool_lock_);
	for (int i = 0; i<=dic_conn_num;i++){
		base_storage::DictionaryStorageEngine* engine =
				base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);

			if (engine==NULL){
				assert(0);
				continue;
			}
			MIG_DEBUG(USER_LEVEL, "ip:%s,port:%d", addrlist_.front().host().c_str(),
					addrlist_.front().port());
			bool r =  engine->Connections(addrlist_);
			if (!r)
				continue;

		dic_conn_pool_.push_back(engine);
	}

#endif
}

void RedisComm::Dest(){
#if defined (_DIC_POOL_)
	logic::WLockGd lk(dic_pool_lock_);
	while(dic_conn_pool_.size()>0){
		base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
		dic_conn_pool_.pop_front();
		if(engine){
			engine->Release();
			delete engine;
			engine =NULL;
		}
	}
	DeinitThreadrw(dic_pool_lock_);
#endif
}

#if defined (_DIC_POOL_)

void RedisComm::RedisConnectionPush(base_storage::DictionaryStorageEngine* engine){
	logic::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	logic::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif




base_storage::DictionaryStorageEngine* RedisComm::GetConnection(){

	try{
		base_storage::DictionaryStorageEngine* engine =
				logic::ThreadKey::GetStorageDicConn();
		if (engine){
// 			if (!engine->){
// 				LOG_ERROR("Database %s connection was broken");
// 				engine->Release();
// 				if (engine){
// 					delete engine;
// 					engine = NULL;
// 				}
// 			}else
				return engine;
		}

		engine =
			base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);

		if (engine==NULL){
			assert(0);
			return NULL;
		}
		bool r =  engine->Connections(addrlist_);
		if (!r)
			return NULL;
		logic::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool RedisComm::GetMusicInfos(const int64 songid,std::string& musicinfo) {
	char* value;
	size_t value_len = 0;
	char key[256] = {0};
	size_t key_length = strlen(key);
	snprintf(key, arraysize(key), "%lld", songid);
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;
	bool r = redis_engine_->GetValue(key,key_length,
		&value,&value_len);
	if (r){
		musicinfo.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",key);
	}
	return true;
}




}
