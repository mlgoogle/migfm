/*
 * redis_controller.cc
 *
 *  Created on: 2015年5月21日
 *      Author: Administrator
 */
#include "redis_controller.h"
#include "dic/base_dic_redis_auto.h"

namespace base_logic{



void RedisController::Release(){

}


bool RedisController::WriteData(const int32 type,base_logic::Value* value){

	switch(type){
	default:
		break;
	}
	return true;
}

bool RedisController::ReadData(const int32 type,base_logic::Value* value,
		void (*storage_get)(void*,base_logic::Value*)){
	switch(type){
	case HASH_VALUE:
		ReadHashData(value,storage_get);
		break;
	default:
		break;
	}
	return true;
}


bool RedisController::ReadKeyValueData(base_logic::Value* value,
		void (*storage_get)(void*,base_logic::Value*)){
	bool r = false;
	std::string key;
	base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)(value);
	r = dict->GetString(L"rediskey",&key);
	if(!r)
		return r;
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine = auto_engine.GetDicEngine();
	//r = redis_engine->GetAllHash()
	return true;
}

bool RedisController::ReadHashData(base_logic::Value* value,
			void (*storage_get)(void*,base_logic::Value*)){
	bool r = false;
	std::list<std::string> list;
	base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)(value);
	std::string redis_map;
	r = dict->GetString(L"redismap",&redis_map);
	if(!r)
		return r;
	base_logic::ListValue* list_value = new base_logic::ListValue();
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine = auto_engine.GetDicEngine();
	r = redis_engine->GetHashValues(redis_map.c_str(),redis_map.length(),list);
	if(!r)
		return false;
	while(list.size()>0){
		std::string info = list.front();
		list.pop_front();
		list_value->Append(base_logic::Value::CreateStringValue(info));
	}
	dict->GetList(L"resultvalue",&list_value);
	return true;
}




}
