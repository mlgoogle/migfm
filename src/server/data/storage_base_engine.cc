/*
 * storage_engine.cc
 *
 *  Created on: 2015年5月21日
 *      Author: Administrator
 */
#include "storage_base_engine.h"
#include "redis_data_storage.h"、
#include "mysql_data_storage.h"
#include "dic/base_dic_redis_auto.h"
#include "db/base_db_mysql_auto.h"

namespace base_logic{

DataStorageBaseEngine* DataStorageBaseEngine::Create(int32 type){
	DataStorageBaseEngine* engine = NULL;
	switch(type){
	case REIDS_TYPE:
		engine = new RedisDatalStorage();
		break;
	case MYSQL_TYPE:
		engine = new MysqlDataStorage();
		break;
	default:
		break;
	}

	return engine;
}

void DataStorageBaseEngine::Init(config::FileConfig* config){
	base_dic::RedisPool::Init(config->redis_list_);
	base_db::MysqlDBPool::Init(config->mysql_db_list_);
}

void DataStorageBaseEngine::Dest(){
	base_dic::RedisPool::Dest();
	base_db::MysqlDBPool::Dest();
}

}



