#include "dic_comm.h"
#include "logic_comm.h"
#include <assert.h>
#include <sstream>

namespace storage{

std::list<base::ConnAddr>  RedisComm::addrlist_;
void RedisComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void RedisComm::Dest(){

}

base_storage::DictionaryStorageEngine* RedisComm::GetConnection() {
	try{
		base_storage::DictionaryStorageEngine* engine =
			mig_lbs::ThreadKey::GetStorageDicConn();
		if (engine)
			return engine;

		engine = 
			base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		MIG_DEBUG(USER_LEVEL, "ip:%s,port:%d", addrlist_.front().host().c_str(),
				addrlist_.front().port());
		engine->Connections(addrlist_);
		mig_lbs::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool RedisComm::GetMusicInfos(const std::string &key, std::string &music_infos){
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

bool RedisComm::IsCollectSong(const std::string& uid,const std::string& songid)
{
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

bool RedisComm::GetMusicAboutUser(const std::string &songid,std::string& content)
{
	char* value;
	size_t value_len = 0;
	std::string key;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;
	key.append("a");
	key.append(songid.c_str());
	key.append("t");
	LOG_DEBUG2("%%%%%key[%s]%%%%%%",key.c_str());
	bool r = redis_engine_->GetValue(key.c_str(),key.length(),
		&value,&value_len);
	if (r){
		content.assign(value,value_len-1);
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

bool MemComm::SetString(const char* key,const size_t key_len,
						const char* data, size_t len){
	 return engine_->SetValue(key,key_len,data,len);
}

bool MemComm::GetString(const char* key,const size_t key_len,
						char** data,size_t* len){

     return engine_->GetValue(key,key_len,data,len);
}

bool MemComm::GetUserCurrentSong(const std::vector<std::string> &vec_user,
		std::map<std::string, std::string> &map_songs) {
	using namespace std;

	map_songs.clear();

	if (vec_user.empty())
		return true;

	typedef std::map<std::string, std::string> KeyUserMap;
	KeyUserMap map_key_users;
	std::vector<const char *> vec_keys;
	std::vector<size_t> vec_key_lens;
	std::stringstream ss;
	for (vector<string>::const_iterator it=vec_user.begin(); it!=vec_user.end(); ++it) {
		ss.str("");
		ss << "cur" << it->c_str();
		const string &key = map_key_users.insert(KeyUserMap::value_type(ss.str(), *it)).first->first;
		LOG_DEBUG2("key [%s]",key.c_str());
		vec_keys.push_back(key.c_str());
		vec_key_lens.push_back(key.size());
	}

	engine_->MGetValue(&vec_keys[0], &vec_key_lens[0], vec_keys.size());

	char return_key[MEMCACHED_MAX_KEY] = {0};
	size_t return_key_length = MEMCACHED_MAX_KEY;
	char *return_value = NULL;
	size_t return_value_length = 0;

	while (engine_->FetchValue(return_key, &return_key_length, &return_value,
	                                      &return_value_length)) {
		if (return_key && return_value){
			map_songs[map_key_users[return_key]] = return_value;
			MIG_DEBUG(USER_LEVEL,"key[%s] value[%s]",return_key,return_value);
		}
		if (return_value)
			free(return_value);
	}
	return true;
}
}

