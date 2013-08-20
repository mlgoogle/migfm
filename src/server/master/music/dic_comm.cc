#include "dic_comm.h"
#include "logic_comm.h"
#include <assert.h>

namespace storage{

std::list<base::ConnAddr>  RedisComm::addrlist_;
base::MigRadomIn* RedisComm::radom_num_ = NULL;
void RedisComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
	radom_num_ = new base::MigRadomIn();
	base::SysRadom::InitRandom();
}

void RedisComm::Dest(){
	if (radom_num_){
		delete radom_num_;
		radom_num_ = NULL;
	}
	base::SysRadom::DeinitRandom();
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
	int num = radom_num_->GetPrize();
	//int num = base::SysRadom::GetRandomID();
	r = redis_engine_->GetHashRadomElement(art_name.c_str(),&value,&value_len,num);
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

//获取心绪图
bool RedisComm:: GetUserMoodMap(const std::string& uid,std::string& mood_map){
	char* value;
	size_t value_len = 0;
	//key uid_mmp
	std::string temp_key = uid;
	temp_key.append("_mmp");
	//value
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	//value 以协议json格式存储
	//{"day":"1","typeid:"1"},{"day":"2","typeid:"2"},{"day":"3","typeid:"1"},{"day":"4","typeid:"3"},{"day":"5","typeid:"2"},{"day":"6","typeid:"5"},{"day":"7","typeid:"6"},

	if (redis_engine_==NULL)
		return true;
	bool r = redis_engine_->GetValue(temp_key.c_str(),temp_key.length(),
		                             &value,&value_len);
	if (r){
		mood_map.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",temp_key.c_str());
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

bool RedisComm::SetCollectSong(const std::string &uid,const std::string& songid){

	//key:hash-huid_clt
	//std::stringstream os;
	std::string os;
	bool r = false;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	//map
	//os<<"h"<<uid.c_str()<<"clt";
	os.append("h");
	os.append(uid.c_str());
	os.append("clt");
	redis_engine_->AddHashElement(os.c_str(),songid.c_str(),songid.length(),
		                          songid.c_str(),songid.length());
	return true;
}

bool RedisComm::GetCollectSongs(const std::string& uid,std::list<std::string>& song_list){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	os.append("h");
	os.append(uid.c_str());
	os.append("clt");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);

	return r;

}

bool RedisComm::DelCollectSong(const std::string& uid,const std::string& songid){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	os.append("h");
	os.append(uid.c_str());
	os.append("clt");

	return  redis_engine_->DelHashElement(os.c_str(),
		                                  songid.c_str(),
										  songid.length());
}

bool RedisComm::IsCollectSong(const std::string& uid,const std::string& songid){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	os.append("h");
	os.append(uid.c_str());
	os.append("clt");

	r = redis_engine_->GetHashElement(os.c_str(),songid.c_str(),songid.length(),
		&value,&value_len);
	if (r){
		if (value){
			free(value);
			value = NULL;
		}
		return true;

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",songid.c_str());
	}

	return false;
}


bool RedisComm::SetHateSong(const std::string &uid, const std::string &songid){

	//key: huid_ht
	std::string os;
	bool r = false;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;
	//map

	os.append("h");
	os.append(uid.c_str());
	os.append("ht");

	redis_engine_->AddHashElement(os.c_str(),songid.c_str(),songid.length(),
		songid.c_str(),songid.length());
	return true;
}

bool RedisComm::DelHateSong(const std::string &uid, const std::string &songid){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	os.append("h");
	os.append(uid.c_str());
	os.append("ht");

	return  redis_engine_->DelHashElement(os.c_str(),
		songid.c_str(),
		songid.length());
}

bool RedisComm::IsHateSong(const std::string& uid,const std::string& songid){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	//os<<"h"<<uid.c_str()<<"ht";
	os.append("h");
	os.append(uid.c_str());
	os.append("ht");

	r = redis_engine_->GetHashElement(os.c_str(),songid.c_str(),songid.length(),
		&value,&value_len);
	if (r){
		if (value){
			free(value);
			value = NULL;
		}
		return true;

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",songid.c_str());
	}

	return false;
}

bool RedisComm::GetDefaultSongs(const std::string &uid, 
								std::list<std::string> &song_list){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
	//duidsl(default+uid+songlist)
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;

	os.append("d");
	os.append(uid.c_str());
	os.append("sl");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);

	return r;
}

bool RedisComm::MgrListenSongsNum(const std::string& songid,const std::string& uid,
								  const int32 flag){
	//key num_songid:num_99999
	std::string os;
	bool r = false;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;
	os.append("num_");
	os.append(songid.c_str());
	LOG_DEBUG2("MgrListenSongsNum key[%s]",os.c_str());
	if (flag)//1
		//r = redis_engine_->IncrValue(os.c_str(),os.length(),NULL,0);
		r = redis_engine_->AddHashElement(os.c_str(),uid.c_str(),uid.length(),
		                                   songid.c_str(),songid.length());
	else
		//r = redis_engine_->DecrValue(os.c_str(),os.length(),NULL,0);
		r = redis_engine_->DelHashElement(os.c_str(),uid.c_str(),uid.length());
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


bool MemComm::SetUsrCurrentSong(const std::string& uid, 
								const std::string& songid){

	//key cur+uid cur100000
	bool r = false;
	std::string key = "cur";
	key.append(uid);
	r = engine_->SetValue(key.c_str(),key.length(),
							songid.c_str(),songid.length());
}
}