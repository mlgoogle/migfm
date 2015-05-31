/*
 * data_redis_engine.cc
 *
 *  Created on: 2015年5月31日
 *      Author: pro
 */
#include "data_redis_engine.h"

namespace base_logic{

DataMusicReidsEngine::DataMusicReidsEngine(){
	redis_engine_.reset(base_logic::DataStorageBaseEngine::Create(REIDS_TYPE));
}

DataMusicReidsEngine::~DataMusicReidsEngine(){

}


bool DataMusicReidsEngine::GetDimensionMusic(const std::string& class_name,const int64 id,
		DIMENSION_MAP& map,DIMENSION_VEC& vec){
	std::string keymap;
	bool r = false;
	base_logic::ListValue* list;
	keymap = class_name+std::string("_r")+base::BasicUtil::BasicUtil::StringUtil::Int64ToString(id);
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	dict->SetString(L"redismap",keymap);
	r = redis_engine_->ReadData(HASH_VALUE,(base_logic::Value*)(dict.get()),NULL);
	if(!r)
		return r;
	while(list->GetSize()>0){
		base_logic::MusicInfo info;
		std::string str;
		base_logic::Value* result_value;
		list->Remove(0,&result_value);
		r = result_value->GetAsString(&str);
		if(!r)
			continue;
		info.set_id(atoll(str.c_str()));
		info.set_class_name(class_name);
		info.set_class(id);
		map[info.id()] = info;
		vec.push_back(info);
	}
	return true;
}
}



