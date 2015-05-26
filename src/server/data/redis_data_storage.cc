/*
 * redis_crawl_storage.cc
 *
 *  Created on: 2015年5月21日
 *      Author: Administrator
 */
#include "redis_data_storage.h"
#include "dic/base_dic_redis_auto.h"

namespace base_logic{



void RedisDatalStorage::Release(){

}


bool RedisDatalStorage::WriteData(const int32 type,base_logic::Value* value){

	switch(type){
	default:
		break;
	}
	return true;
}


}
