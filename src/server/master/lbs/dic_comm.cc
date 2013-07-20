#include "dic_comm.h"
#include "logic_comm.h"
#include <assert.h>

namespace storage{

std::list<base::ConnAddr>  RedisComm::addrlist_;
void RedisComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void RedisComm::Dest(){

}

base_storage::DictionaryStorageEngine* RedisComm::GetConnection() {
	try{
		base_storage::DictionaryStorageEngine* engine =
			mig_lbs::ThreadKey::GetStorageDicConn();
		if (engine)
			return engine;

		engine = 
			base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		MIG_DEBUG(USER_LEVEL, "ip:%s,port:%d", addrlist_.front().host().c_str(),
				addrlist_.front().port());
		engine->Connections(addrlist_);
		mig_lbs::ThreadKey::SetStorageDicConn(engine);
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
