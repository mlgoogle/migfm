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

base_logic::DataWholeManager *GetWholeManager(void){
	return base_logic::DataCacheEngine::GetWholeManager();
}

namespace base_logic{


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

bool DataWholeManager::DelUserInfo(const int64 uid){
	base_logic::WLockGd lk(lock_);
	return base::MapDel<USER_INFO_MAP,USER_INFO_MAP::iterator,int64>(data_cache_->userinfo_map_,uid);
}



}


