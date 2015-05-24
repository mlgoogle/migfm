/*
 * data_cache_manager.cc
 *
 *  Created on: 2015年5月19日
 *      Author: pro
 */
#include "data_cache_manager.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include <list>

base_logic::DataEngine *GetDateEngine(void){
	return new base_logic::DateEngineImpl();
}


namespace base_logic{


bool DateEngineImpl::SetUserInfo(const int64 uid,base_logic::UserInfo& info){
	return DataCacheEngine::GetWholeManager()->SetUserInfo(uid,info);
}

bool DateEngineImpl::DelUserInfo(const int64 uid){
	return DataCacheEngine::GetWholeManager()->DelUserInfo(uid);
}


DataWholeManager* DataCacheEngine::whole_mgr_ = NULL;
DataCacheEngine* DataCacheEngine::data_cache_engine_ = NULL;


DataWholeManager::DataWholeManager(){
	data_cache_ = new DataCache();
	Init();
}

DataWholeManager::~DataWholeManager(){

}


void DataWholeManager::Init(std::list<base::ConnAddr>& addrlist){

}

void DataWholeManager::Init(){
	InitThreadrw(&lock_);
}

bool DataWholeManager::SetUserInfo(const int64 uid,base_logic::UserInfo& info){
	base_logic::WLockGd lk(lock_);
	return base::MapAdd<USER_INFO_MAP,int64,base_logic::UserInfo>(data_cache_->userinfo_map_,uid,info);
}

bool DataWholeManager::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
	bool r = false;
	{
		base_logic::RLockGd lk(lock_);
		r = base::MapGet<USER_INFO_MAP,USER_INFO_MAP::iterator,int64,base_logic::UserInfo>
		(data_cache_->userinfo_map_,uid,info);
	}
	if(r)
		return r;
	//从memcached 读取
	//从数据库里读取
	return true;
}

bool DataWholeManager::DelUserInfo(const int64 uid){
	base_logic::WLockGd lk(lock_);
	LOG_DEBUG("HAHAHAHAHAHAHAHAHAHA");
	return base::MapDel<USER_INFO_MAP,USER_INFO_MAP::iterator,int64>(data_cache_->userinfo_map_,uid);
}



}


