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
#include "storage_operation.h"
#include "data_db_comm.h"
#include "dict_comm.h"
namespace base_logic{


StorageOperation* StorageOperation::instance_ = NULL;
StorageOperation::StorageOperation(){

}

StorageOperation::~StorageOperation(){

}

StorageOperation* StorageOperation::Instance(){
	if(instance_==NULL){
		instance_ = new StorageOperation();
	}

	return instance_;
}

void StorageOperation::Init(config::FileConfig* config){
	base_logic::UserMemComm::Init(config->mem_list_);
	base_logic::DataDBComm::Init(config->mysql_db_list_);
}


bool StorageOperation::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
	bool r = base_logic::UserMemComm::GetUserInfo(uid,info);
	if(r)
		return r;
	//数据库读取
	r = base_logic::DataDBComm::GetUserInfo(uid,info);
	if(!r)
		return r;
	//写入memcached
	r = base_logic::UserMemComm::SetUserInfo(uid,info);
	return true;
}

}


