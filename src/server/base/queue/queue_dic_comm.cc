/*
 * queue_dic_comm.cc
 *
 *  Created on: 2015年1月3日
 *      Author: kerry
 */
#include "queue_dic_comm.h"
#include "dic/base_dic_redis_auto.h"
namespace base_queue{


QueueDicComm::QueueDicComm(){

}

QueueDicComm::~QueueDicComm(){
	Dest();
}

void QueueDicComm::Init(std::list<base::ConnAddr>& addrlist){
	base_dic::RedisPool::Init(addrlist);
}

void QueueDicComm::Dest(){
	base_dic::RedisPool::Dest();
}

bool QueueDicComm::AddBlockQueueMessage(const std::string& key,const std::string& str){
	//list
	bool r = false;
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
	r =  redis_engine_->AddListElement(key.c_str(),key.length(),str.c_str(),str.length(),1);
	return r;
}


}



