/*
 * data_cache_manager.cc
 *
 *  Created on: 2015年5月19日
 *      Author: pro
 */
#include "data_cache_manager.h"
#include "storage_operation.h"
#include "dict_comm.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include <list>

#define DEFAULT_CONFIG_PATH     "./plugins/usersvc/usersvc_config.xml"

base_logic::DataEngine *GetDateEngine(void){
	return new base_logic::DateEngineImpl();
}


namespace base_logic{

void GetInfoMapTraits::BatchGetUsers(std::list<int64>& uid_list,USER_INFO_MAP& usermap,struct threadrw_t* lock,
		Container& container){

}

void GetInfoListTraits::BatchGetUsers(std::list<int64>& uid_list,USER_INFO_MAP& usermap,struct threadrw_t* lock,
		Container& container){

}

//模板
template<typename GetInfoContainerTraits>
bool BatchGetUserInfos(std::list<int64>& uid_list,USER_INFO_MAP& usermap,struct threadrw_t* lock,
		typename GetInfoContainerTraits::Container& container){
	typedef GetInfoContainerTraits traits;
	return traits::BatchGetUsers(uid_list,usermap,lock,container);
}

/////////////////////////////////////////////////////////////////////////////

bool DateEngineImpl::SetUserInfo(const int64 uid,base_logic::UserInfo& info){
	return DataCacheEngine::GetWholeManager()->SetUserInfo(uid,info);
}

bool DateEngineImpl::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
	return DataCacheEngine::GetWholeManager()->GetUserInfo(uid,info);
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
	//
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return ;
	}
	r = config->LoadConfig(path);

	base_logic::StorageOperation::Instance()->Init(config);
}



bool DataWholeManager::SetUserInfo(const int64 uid,base_logic::UserInfo& info){
	base_logic::WLockGd lk(lock_);
	base_logic::UserMemComm::SetUserInfo(uid,info);
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

	base_logic::StorageOperation::Instance()->GetUserInfo(uid,info);

	{
		base_logic::WLockGd lk(lock_);
		base::MapAdd<USER_INFO_MAP,int64,base_logic::UserInfo>(data_cache_->userinfo_map_,uid,info);
	}
	return true;
}


bool DataWholeManager::DelUserInfo(const int64 uid){
	base_logic::WLockGd lk(lock_);
	return base::MapDel<USER_INFO_MAP,USER_INFO_MAP::iterator,int64>(data_cache_->userinfo_map_,uid);
}






}


