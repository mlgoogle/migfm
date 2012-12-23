#include "storage/dic_serialization.h"
#include "basic/basic_info.h"
#include "storage/storage.h"
#include "storage/mem_storage_impl.h"
#include "storage/redis_storage_impl.h"
#include <assert.h>

namespace base_storage{

base_storage::DictionaryStorageEngine*  
    MemDicSerial::mem_engine_ = NULL;
	
base_storage::DictionaryStorageEngine*  
	RedisDicSerial::redis_engine_ = NULL;
		
		
///memcached
bool MemDicSerial::Init(std::list<base::ConnAddr>& addrlist){
    mem_engine_ = DictionaryStorageEngine::Create(IMPL_MEM);
    assert(mem_engine_!=NULL);	
    return mem_engine_->Connections(addrlist);
}

bool MemDicSerial::SetString(const char* key,const size_t key_len,
                             const char* data,size_t len){
   return mem_engine_->SetValue(key,key_len,data,len);
}

bool MemDicSerial::GetString(const char* key,const size_t key_len,
                             char** data,size_t* len){
   return mem_engine_->GetValue(key,key_len,data,len);
}

//redis
bool RedisDicSerial::Init(std::list<base::ConnAddr>& addrlist){
	redis_engine_ = DictionaryStorageEngine::Create(IMPL_RADIES);
	assert(redis_engine_ != NULL);
	return redis_engine_->Connections(addrlist);
}

}
