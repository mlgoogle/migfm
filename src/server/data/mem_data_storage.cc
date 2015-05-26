/*
 * mem_data_storage.cc
 *
 *  Created on: 2015年5月26日
 *      Author: kerry
 *  Time: 下午11:03:33
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */
#include "mem_data_storage.h"
#include "dic/base_dic_redis_auto.h"

namespace base_logic{



void MemDatalStorage::Release(){

}


bool MemDatalStorage::WriteData(const int32 type,base_logic::Value* value){

	switch(type){
	default:
		break;
	}
	return true;
}

bool MemDatalStorage::WriteKeyValueData(base_logic::Value* value){
	return true;
}

}



