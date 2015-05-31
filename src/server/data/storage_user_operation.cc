/*
 * storage_operation.cc
 *
 *  Created on: 2015年5月25日
 *      Author: kerry
 *  Time: 下午10:48:17
 *  Project: migfm
 *  Year: 2015
 *  Instruction：从存储介质中获取用户信息
 */
#include "storage_user_operation.h"
#include "storage_base_engine.h"
#include "data_mem_engine.h"
#include "data_mysql_engine.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
namespace base_logic{


StorageUserOperation* StorageUserOperation::instance_ = NULL;
StorageUserOperation::StorageUserOperation(){

	user_engine_.reset(new base_logic::DataMysqlEngne());
	mem_engine_.reset(new base_logic::DataUserMemEngine());

}

StorageUserOperation::~StorageUserOperation(){

}

StorageUserOperation* StorageUserOperation::Instance(){
	if(instance_==NULL){
		instance_ = new StorageUserOperation();
	}

	return instance_;
}

void StorageUserOperation::Init(config::FileConfig* config){
	base_logic::DataStorageBaseEngine::Init(config);
	mem_engine_->Init(config->mem_list_);

	//base_logic::UserMemComm::Init(config->mem_list_);
	//base_logic::DataDBComm::Init(config->mysql_db_list_);
}


bool StorageUserOperation::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
	bool r = mem_engine_->GetUserInfo(uid,info);
	if(r)
		return r;

	r = user_engine_->ReadUserInfo(uid,info);
	if(!r)
		return r;
	r = mem_engine_->SetUserInfo(uid,info);
	/*bool r = base_logic::UserMemComm::GetUserInfo(uid,info);
	if(r)
		return r;
	//数据库读取
	r = base_logic::DataDBComm::GetUserInfo(uid,info);
	if(!r)
		return r;
	//写入memcached
	r = base_logic::UserMemComm::SetUserInfo(uid,info);*/
	return true;
}

bool StorageUserOperation::BatchGetUserInfo(std::vector<int64>& uid_list,
		std::map<int64,base_logic::UserInfo>& userinfo){
	bool r = mem_engine_->BatchGetUserInfo(uid_list,userinfo);
	std::vector<int64> surplus_list;
	//检测是否完整
	int32 i = 0;
	while(i<uid_list.size()){
		base_logic::UserInfo info;
		if(!base::MapGet<USER_INFO_MAP,USER_INFO_MAP::iterator,int64,base_logic::UserInfo>(userinfo,uid_list[i],info))
			surplus_list.push_back(uid_list[i]);
		i++;
	}
	if(surplus_list.size()>0)
		user_engine_->BatchReadUserInfos(surplus_list,userinfo);

	//批量写入 //libmemcached 暂无批量写入接口
	i = 0;
	while(i<surplus_list.size()){
		base_logic::UserInfo info;
		if(base::MapGet<USER_INFO_MAP,USER_INFO_MAP::iterator,int64,base_logic::UserInfo>(userinfo,surplus_list[i],info))
			mem_engine_->SetUserInfo(surplus_list[i],info);
		i++;
	}
	return true;
}

}


