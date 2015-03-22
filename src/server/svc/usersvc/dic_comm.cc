/*
 * dic_comm.cc
 *
 *  Created on: 2015年3月6日
 *      Author: pro
 */

#include "dic_comm.h"
#include "dic/base_dic_redis_auto.h"
#include <string>

namespace usersvc_logic{

UserDicComm::UserDicComm(){

}

UserDicComm::~UserDicComm(){
	Dest();
}

void UserDicComm::Init(std::list<base::ConnAddr>& addrlist){
	base_dic::RedisPool::Init(addrlist);
}

void UserDicComm::Dest(){
	base_dic::RedisPool::Dest();
}

void UserDicComm::GetNewMsgNum(const int64 uid, int32& msg_num){
	char key[256] = {0};
	char* value;
	size_t value_len = 0;
	snprintf(key,arraysize(key),"soc%lld:new.msg",uid);
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();

	bool r = redis_engine_->GetValue(key,strlen(key),&value,&value_len);
	if(r){
		msg_num = atol(value);
		if(value){free(value);value=NULL;}
	}
}


}



