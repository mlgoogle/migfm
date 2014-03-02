#include "storage/dic_storage.h"
#include "basic/basic_info.h"
#include "storage/storage.h"
#include <assert.h>
#include <sstream>

namespace base_storage{

base_storage::DictionaryStorageEngine*  
		MemDic::mem_engine_ = NULL;

	///memcached
bool MemDic::Init(std::list<base::ConnAddr>& addrlist){
	mem_engine_ = DictionaryStorageEngine::Create(IMPL_MEM);
	assert(mem_engine_!=NULL);	
	return mem_engine_->Connections(addrlist);
}

bool MemDic::SetString(const char* key,const size_t key_len,
	const char* data,size_t len){
		return mem_engine_->SetValue(key,key_len,data,len);
}

bool MemDic::GetString(const char* key,const size_t key_len,
	char** data,size_t* len){
		return mem_engine_->GetValue(key,key_len,data,len);
}

}