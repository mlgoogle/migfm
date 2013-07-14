#include "dic_comm.h"
#include "logic_comm.h"
#include <assert.h>

namespace storage{

std::list<base::ConnAddr>  RedisComm::addrlist_;
void RedisComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void RedisComm::Dest(){

}

base_storage::DictionaryStorageEngine* RedisComm::GetConnection(){

	try{
		base_storage::DictionaryStorageEngine* engine =
			usr_logic::ThreadKey::GetStorageDicConn();
		if (engine){
// 			if (!engine->){
// 				LOG_ERROR("Database %s connection was broken");
// 				engine->Release();
// 				if (engine){
// 					delete engine;
// 					engine = NULL;
// 				}
// 			}else
				return engine;
		}

		engine = 
			base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);
		engine->Connections(addrlist_);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		usr_logic::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}



bool RedisComm::GetMoodAndScensId(const std::string &key, 
								  std::string &word_id){
   return true;
}

bool RedisComm::GetMusicMapRadom(const std::string &art_name, 
								 std::string &song_id){
	char* value;
	size_t value_len = 0;
	bool r = false;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	r = redis_engine_->GetHashRadomElement(art_name.c_str(),&value,&value_len);
	if (r){
	 song_id.assign(value,value_len-1);
	 if (value){
		 free(value);
		 value = NULL;
	 }

	}else{
	 MIG_ERROR(USER_LEVEL,"GetValue error");
	}
	return r;
}

bool RedisComm::GetMusicInfos(const std::string& key,std::string& music_infos){
	char* value;
	size_t value_len = 0;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;
	bool r = redis_engine_->GetValue(key.c_str(),key.length(),
		&value,&value_len);
	if (r){
		music_infos.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",key.c_str());
	}

	return r;
}
/////////////////////////////////memcahced//////////////////////////////////////
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

}