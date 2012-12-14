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
void MemDicSerial::Init(std::list<base::ConnAddr>& addrlist){
    mem_engine_ = DictionaryStorageEngine::Create(IMPL_MEM);
    assert(mem_engine_==NULL);	
    mem_engine_->Connections(addrlist);
}



//redis
void RedisDicSerial::Init(std::list<base::ConnAddr>& addrlist){
	redis_engine_ = DictionaryStorageEngine::Create(IMPL_RADIES);
	assert(redis_engine_ = NULL);
	redis_engine_->Connections(addrlist);
}

}