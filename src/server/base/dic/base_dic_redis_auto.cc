/*
 * base_dic_redis_auto.cc
 *
 *  Created on: 2014年12月22日
 *      Author: kerry
 */
#include "base_dic_redis_auto.h"
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"
namespace base_dic{

threadrw_t* RedisPool::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisPool::dic_conn_pool_;

std::list<base::ConnAddr> RedisPool::addrlist_;

AutoDicCommEngine::AutoDicCommEngine(){
	engine_ = base_dic::RedisPool::RedisConnectionPop();
}

AutoDicCommEngine::~AutoDicCommEngine(){
	base_dic::RedisPool::RedisConnectionPush(engine_);
}


void RedisPool::Init(std::list<base::ConnAddr>& addrlist,const int32 conn_num){
	bool r =false;
	addrlist_ = addrlist;
	InitThreadrw(&dic_pool_lock_);
	for (int i = 0; i<=conn_num;i++){
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
}


void RedisPool::Dest(){
	base_logic::WLockGd lk(dic_pool_lock_);
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
}

void RedisPool::RedisConnectionPush(base_storage::DictionaryStorageEngine* engine){
	base_logic::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisPool::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	base_logic::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

}



