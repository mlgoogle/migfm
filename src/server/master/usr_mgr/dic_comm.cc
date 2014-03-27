#include "dic_comm.h"
#include "logic_comm.h"
#include <assert.h>

namespace storage{

std::list<base::ConnAddr>  RedisComm::addrlist_;

#if defined (_DIC_POOL_)
threadrw_t* RedisComm::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisComm::dic_conn_pool_;
#endif


AutoDicCommEngine::AutoDicCommEngine()
:engine_(NULL){
#if defined (_DIC_POOL_)
	engine_ = storage::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DIC_POOL_)
	storage::RedisComm::RedisConnectionPush(engine_);
#endif
}

void RedisComm::Init(std::list<base::ConnAddr>& addrlist,const int32 dic_conn_num){
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
	mig_lbs::WLockGd lk(dic_pool_lock_);
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
	mig_lbs::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	mig_lbs::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif


base_storage::DictionaryStorageEngine* RedisComm::GetConnection(){

	try{
		base_storage::DictionaryStorageEngine* engine =
			usr_logic::ThreadKey::GetStorageDicConn();
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
		bool r = engine->Connections(addrlist_);
		if (!r)
			return NULL;

		usr_logic::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}




/////////////////////////////////memcahced//////////////////////////////////////
base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;

void MemComm::Init(std::list<base::ConnAddr>& addrlist){
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	engine_->Connections(addrlist);
}

void MemComm::Dest(){
	if (engine_){
		delete engine_;
		engine_ = NULL;
	}
}

bool MemComm::SetString(const char* key,const size_t key_len,
						const char* data, size_t len){
	 return engine_->SetValue(key,key_len,data,len);
}

bool MemComm::GetString(const char* key,const size_t key_len,
						char** data,size_t* len){

     return engine_->GetValue(key,key_len,data,len);
}

}
