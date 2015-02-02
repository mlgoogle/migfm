/*
 * dic_comm.cc
 *
 *  Created on: 2015年1月22日
 *      Author: mac
 */
#include "dic_comm.h"
#include "dic/base_dic_redis_auto.h"

static const char* KEY_PUSH_NEW_MSG_NUM = "soc:%lld:new.msg";
namespace msgsvc_logic{


DicComm::DicComm(){

}

DicComm::~DicComm(){
	Dest();
}

void DicComm::Init(std::list<base::ConnAddr>& addrlist){
	base_dic::RedisPool::Init(addrlist);
}

void DicComm::Dest(){
	base_dic::RedisPool::Dest();
}

bool DicComm::GetNewCount(const int64 tid, int64& new_msg){
	bool r = false;
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_PUSH_NEW_MSG_NUM, tid);
	return redis_engine_->IncDecValue(key, strlen(key), 1, new_msg);
}

}


