#include "dic_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <assert.h>
#include <sstream>

namespace storage{


#if defined (_DIC_POOL_)
threadrw_t* RedisComm::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisComm::dic_conn_pool_;
#endif


std::list<base::ConnAddr>  RedisComm::addrlist_;

AutoDicCommEngine::AutoDicCommEngine()
:engine_(NULL){
#if defined (_DB_POOL_)
	engine_ = storage::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DB_POOL_)
	storage::RedisComm::RedisConnectionPush(engine_);
#endif
}

void RedisComm::Init(std::list<base::ConnAddr>& addrlist,const int32 dic_conn_num){
	addrlist_ = addrlist;

#if defined (_DIC_POOL_)
	bool r =false;
	InitThreadrw(&dic_pool_lock_);
	for (int i = 0; i<=dic_conn_num;i++){
		base_storage::DictionaryStorageEngine* engine =
				base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);

			if (engine==NULL){
				assert(0);
				continue;
			}
			MIG_DEBUG(USER_LEVEL, "ip:%s,port:%d", addrlist_.front().host().c_str(),
					addrlist_.front().port());
			bool r =  engine->Connections(addrlist_);
			if (!r)
				continue;

		dic_conn_pool_.push_back(engine);
	}

#endif
}

void RedisComm::Dest(){
#if defined (_DIC_POOL_)
	mig_lbs::WLockGd lk(dic_pool_lock_);
	while(dic_conn_pool_.size()>0){
		base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
		dic_conn_pool_.pop_front();
		if(engine){
			engine->Release();
			delete engine;
			engine =NULL;
		}
	}
	DeinitThreadrw(dic_pool_lock_);
#endif
}

#if defined (_DIC_POOL_)

void RedisComm::RedisConnectionPush(base_storage::DictionaryStorageEngine* engine){
	mig_lbs::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	mig_lbs::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif

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
		bool r =  engine->Connections(addrlist_);
		if (!r)
			return NULL;
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
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
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

bool RedisComm::GetMsgCount(const std::string& uid,int& count){

	//"soc:%lld:push.msg";
	std::string key;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;
	key.append("soc:");
	key.append(uid);
	key.append(":push.msg");
	count  = redis_engine_->GetListSize(key.c_str());
	return true;

}
bool RedisComm::IsCollectSong(const std::string& uid,const std::string& songid)
{
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
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

	}
	return false;
}

bool RedisComm::GetMusicAboutUser(const std::string &songid,std::string& content)
{
	char* value;
	size_t value_len = 0;
	std::string key;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
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


bool RedisComm::GetCollectSongs(const std::string &uid, 
								std::map<std::string,std::string> &collect){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
	Json::Reader reader;
	Json::Value  root;
	std::list<std::string> song_list;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return false;

	os.append("h");
	os.append(uid.c_str());
	os.append("clt");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);
	if (song_list.size()<=0)
		return false;

	while(song_list.size()>0){
		std::string songinfo = song_list.front();
		song_list.pop_front();
		//����json;
		r = reader.parse(songinfo.c_str(),root);
		if (!r)
			continue;
		if (root.isMember("songid")){
			std::string songid = root["songid"].asString();
			collect[songid] = songinfo;
		}
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

