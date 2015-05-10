/*
 * music_dic_comm.cc
 *
 *  Created on: 2015年3月20日
 *      Author: pro
 */

#include "music_dic_comm.h"
#include "whole_manager.h"
#include "dic/base_dic_redis_auto.h"
#include "basic/basic_util.h"
#include <string>
#include <sstream>

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
	//hash_name = "h10113clt";
	hash_name = "h"+base::BasicUtil::StringUtil::Int64ToString(uid)+"clt";
	std::string key  = base::BasicUtil::StringUtil::Int64ToString(songid);
	SetMusicHashInfo(hash_name,key,json);
}

void MusicDicComm::SetHate(const int64 uid,const int64 songid,const std::string& json){
	std::string hash_name;
	//key: huid_ht
	//hash_name = "h10113ht";
	hash_name = "h"+base::BasicUtil::StringUtil::Int64ToString(uid)+"ht";
	std::string key = base::BasicUtil::StringUtil::Int64ToString(songid);
	SetMusicHashInfo(hash_name,key,json);
}

void MusicDicComm::SetMusicHashInfo(const std::string& hash_name,const std::string& key,
			const std::string& json){
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine = auto_engine.GetDicEngine();
	redis_engine->SetHashElement(hash_name.c_str(),key.c_str(),
			key.length(),json.c_str(),json.length());
}

void MusicDicComm::DelCollect(const int64 uid,const int64 songid){
	/*char key[256] = {0};
	char* value;
	size_t value_len = 0;
	snprintf(key,arraysize(key),"soc%lld:new.msg",uid);*/
	std::string hash_name;
	//hash_name = "h10113clt";
	hash_name = "h"+base::BasicUtil::StringUtil::Int64ToString(uid)+"clt";
	std::string key  = base::BasicUtil::StringUtil::Int64ToString(songid);
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
	redis_engine_->DelHashElement(hash_name.c_str(),key.c_str(),key.length());
}



/////

base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;

void MemComm::Init(std::list<base::ConnAddr>& addrlist){
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	engine_->Connections(addrlist);
}

void MemComm::Dest(){
	if (engine_){
		delete engine_;
		engine_ = NULL;
	}
}

void MemComm::SetUserCurrentMusic(const int64 uid,const std::string& json){
	//key cur+uid cur100000
	bool r = false;
	std::string key = "cur";
	key.append(base::BasicUtil::StringUtil::Int64ToString(uid));
	engine_->SetValue(key.c_str(),key.length(),
			json.c_str(),json.length());
}

void MemComm::BatchGetCurrentSong(std::map<int64,base_logic::UserAndMusic>& map){
	//可批处理提取
	std::stringstream ss;
	bool r = false;
	MUSICINFO_MAP infomap;
	for (std::map<int64,base_logic::UserAndMusic>::const_iterator it=map.begin();
			it!=map.end(); ++it) {
		ss.str("");
		ss << "cur" << it->first;
		const std::string key =ss.str();
		char* data = NULL;
		size_t len = 0;
		r = engine_->GetValue(key.c_str(),key.length(),&data,&len);
		if(r){
			std::string value;
			value.assign(data,len);
			MIG_DEBUG(USER_LEVEL,"key[%s] value[%s]",key.c_str(),data);
			base_logic::UserAndMusic info = it->second;
			info.musicinfo_.JsonSeralize(value);
			//获取完整信息
			infomap[info.musicinfo_.id()] = info.musicinfo_;
		}
		if(data){delete [] data; data = NULL;}
	}
	musicsvc_logic::CacheManagerOp::GetWholeManager()->GetMusicInfo(infomap);


}
}



