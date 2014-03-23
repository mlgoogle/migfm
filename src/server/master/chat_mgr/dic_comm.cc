#include "dic_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <assert.h>


namespace chat_storage{
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

bool MemComm::GetUserToken(const std::string& platform_id,
		          const std::string& uid,std::string& token){
    bool r = false;
	std::string key;
    key.append(platform_id);
    key.append("_");
    key.append(uid);
	size_t value_len = 0;
	char* value = NULL;
	r = engine_->GetValue(key.c_str(),key.length(),&value,&value_len);
	if(r){
		token.assign(value,value_len);
		return true;
	}
	return false;

}
}
