/*
 * pub_dic_comm.cc
 *
 *  Created on: 2015年2月1日
 *      Author: pro
 */

#include "pub_dic_comm.h"
#include "dic/base_dic_redis_auto.h"
#include "basic/basic_util.h"

namespace basic_logic{

PubDicComm::PubDicComm(){

}

PubDicComm::~PubDicComm(){
	Dest();
}

void PubDicComm::Init(std::list<base::ConnAddr>& addrlist){
	base_dic::RedisPool::Init(addrlist);
}

void PubDicComm::Dest(){
	base_dic::RedisPool::Dest();
}

void PubDicComm::GetDimensionMusicList(const std::string& key,std::list<std::string>& list){
	bool r = false;
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
	redis_engine_->GetHashValues(key.c_str(),key.length(),list);
}

void PubDicComm::GetColllectList(const int64 uid,std::list<std::string>& list){
	bool r = false;
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
	std::string key;
	key = "h10113clt";
	//key.append();
	redis_engine_->GetHashValues(key.c_str(),key.length(),list);
}


/*
void PubDicComm::GetBatchMusicInfo(std::list<base_logic::MusicInfo>& map){
	bool r = false;
	base_dic::AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
	std::stringstream os;
	std::string command;
	//
	redisReply *rpl = (redisReply *) redisCommand(context,command.c_str());
	base_storage::CommandReply *reply = base_dic::RedisOperation::_CreateReply(rpl);
	freeReplyObject(rpl);
	if (NULL == reply)
		return ;
	if (base_storage::CommandReply::REPLY_ARRAY == reply->type) {
		base_storage::ArrayReply *arep =
			static_cast<base_storage::ArrayReply *>(reply);
		base_storage::ArrayReply::value_type &items = arep->value;
		for (base_storage::ArrayReply::iterator it = items.begin();
			it != items.end();++it) {
				base_storage::CommandReply *item = (*it);
				if (base_storage::CommandReply::REPLY_STRING == item->type) {
					base_storage::StringReply *srep = static_cast<base_storage::StringReply *>(item);
				}
		}
	}
	reply->Release();

}*/

}
