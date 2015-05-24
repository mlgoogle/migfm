/*
 * dict_comm.cc
 *
 *  Created on: 2015年5月24日
 *      Author: Administrator
 */
#include "dict_comm.h"


static const char *USER_INFO = "userinfo:%lld";

namespace base_logic{

base_storage::DictionaryStorageEngine* BaseMemComm::engine_ = NULL;

void BaseMemComm::Init(std::list<base::ConnAddr>& addrlist){
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	engine_->Connections(addrlist);
}

void BaseMemComm::Dest(){
	if(engine_){
		delete engine_;
		engine_ =NULL;
	}
}

UserMemComm::UserMemComm(){

}

UserMemComm::~UserMemComm(){

}

void UserMemComm::Init(std::list<base::ConnAddr>& addrlist){
	BaseMemComm::Init(addrlist);
}

void UserMemComm::Dest(){
	BaseMemComm::Dest();
}


bool UserMemComm::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
	bool r = false;
	char key[256] = {0};
	char* value = NULL;
	size_t value_len = 0;
	std::string info;
	snprintf(key, arraysize(key), USER_INFO, uid);
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine  = auto_engine.GetDicEngine();
	bool r = redis_engine->GetValue(key,strlen(key),&value,&value_len);
	if(!r||value==NULL||value_len<=0)
		return false;
	info.assign(value,value_len);
	return info.DeSerialization(info);
}


}




