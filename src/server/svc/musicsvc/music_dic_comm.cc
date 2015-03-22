/*
 * music_dic_comm.cc
 *
 *  Created on: 2015年3月20日
 *      Author: pro
 */

#include "music_dic_comm.h"
#include "dic/base_dic_redis_auto.h"
#include "basic/basic_util.h"
#include <string>

namespace musicsvc_logic{

MusicDicComm::MusicDicComm(){

}

MusicDicComm::~MusicDicComm(){
	Dest();
}

void MusicDicComm::Init(std::list<base::ConnAddr>& addrlist){
	base_dic::RedisPool::Init(addrlist);
}

void MusicDicComm::Dest(){
	base_dic::RedisPool::Dest();
}


void MusicDicComm::SetCollect(const int64 uid,const int64 songid,const std::string& json){
	/*char key[256] = {0};
	char* value;
	size_t value_len = 0;
	snprintf(key,arraysize(key),"soc%lld:new.msg",uid);*/
	std::string hash_name;
	hash_name = "h10113clt";
	std::string key  = base::BasicUtil::StringUtil::Int64ToString(songid);
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
	//redis_engine_->SetValue(key.c_str(),key.length(),json.c_str(),json.length());
	redis_engine_->SetHashElement(hash_name.c_str(),key.c_str(),
			key.length(),json.c_str(),json.length());
}

}



