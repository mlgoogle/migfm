/*
 * storage_engine.cc
 *
 *  Created on: 2015年5月21日
 *      Author: Administrator
 */
#include "storage_controller_engine.h"
#include "redis_controller.h"
#include "mem_controller.h"
#include "mysql_controller.h"
#include "dic/base_dic_redis_auto.h"
#include "db/base_db_mysql_auto.h"

namespace base_logic{

DataControllerEngine* DataControllerEngine::Create(int32 type){
	DataControllerEngine* engine = NULL;
	switch(type){
	case REIDS_TYPE:
		engine = new RedisController();
		break;
	case MYSQL_TYPE:
		engine = new MysqlController();
		break;
	case MEM_TYPE:
		engine = new MemController();
		break;
	default:
		break;
	}

	return engine;
}

void DataControllerEngine::Init(config::FileConfig* config){
	base_dic::RedisPool::Init(config->redis_list_);
	base_db::MysqlDBPool::Init(config->mysql_db_list_);
}

void DataControllerEngine::Dest(){
	base_dic::RedisPool::Dest();
	base_db::MysqlDBPool::Dest();
}

}



