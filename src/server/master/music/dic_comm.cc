#include "dic_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <sstream>
#include <assert.h>

namespace storage{

std::list<base::ConnAddr>  RedisComm::addrlist_;
base::MigRadomIn* RedisComm::radom_num_ = NULL;

#if defined (_DIC_POOL_)
threadrw_t* RedisComm::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisComm::dic_conn_pool_;
#endif
AutoDicCommEngine::AutoDicCommEngine()
:engine_(NULL){
#if defined (_DIC_POOL_)
	engine_ = storage::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DIC_POOL_)
	storage::RedisComm::RedisConnectionPush(engine_);
#endif
}

void RedisComm::Init(std::list<base::ConnAddr>& addrlist,const int32 dic_conn_num){
	addrlist_ = addrlist;
    radom_num_ = new base::MigRadomIn();
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
	music_logic::WLockGd lk(dic_pool_lock_);
	while(dic_conn_pool_.size()>0){
		base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
		dic_conn_pool_.pop_front();
		if(engine){
			engine->Release();
			delete engine;
			engine =NULL;
		}
	}
  if (radom_num_){
		delete radom_num_;
		radom_num_ = NULL;
  }
	DeinitThreadrw(dic_pool_lock_);
#endif
}

#if defined (_DIC_POOL_)

void RedisComm::RedisConnectionPush(base_storage::DictionaryStorageEngine* engine){
	music_logic::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	music_logic::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif



base_storage::DictionaryStorageEngine* RedisComm::GetConnection(){

	try{
		base_storage::DictionaryStorageEngine* engine =
				music_logic::ThreadKey::GetStorageDicConn();
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
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		bool r = engine->Connections(addrlist_);
		if (!r)
			return NULL;

		music_logic::ThreadKey::SetStorageDicConn(engine);
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
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
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

//��ȡ����ͼ
bool RedisComm:: GetUserMoodMap(const std::string& uid,std::string& mood_map){
	char* value;
	size_t value_len = 0;
	//key uid_mmp
	std::string temp_key = uid;
	temp_key.append("_mmp");
	//value
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	//value ��Э��json��ʽ�洢
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

bool RedisComm::SetCollectSong(const std::string &uid,const std::string& songid,
							   const std::string& content){

	//key:hash-huid_clt
	//std::stringstream os;
	std::string os;
	bool r = false;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;

	//map
	//os<<"h"<<uid.c_str()<<"clt";
	os.append("h");
	os.append(uid.c_str());
	os.append("clt");
	LOG_DEBUG2("#####key[%s]#######",os.c_str());
	LOG_DEBUG2("#####value[%s]#######",content.c_str());
	redis_engine_->SetHashElement(os.c_str(),songid.c_str(),songid.length(),
		                          content.c_str(),content.length());
	return true;
}

bool RedisComm::GetMusicHistroyCollect(const std::string &uid, 
									   const std::string &is_like, 
									   std::list<std::string>&  songlist, 
									   std::list<std::string>& collect_list, 
									   std::list<std::string>& history_list){
   std::string os;
   bool r = false;
   std::list<std::string> temp_list;
   Json::Value root;
   Json::Reader reader;
   base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
   if (redis_engine_==NULL)
	   return false;
	//��ȡ��ʷ������Ϣ
   GetMusicInfos(redis_engine_,songlist,history_list);
   if(is_like=="1"){
	   //��ȡ���ĸ赥
	   os.append("h");
	   os.append(uid.c_str());
	   os.append("clt");
	   r = redis_engine_->GetHashValues(os.c_str(),os.length(),temp_list);
	   while(temp_list.size()>0){
		   std::string info = temp_list.front();
		   temp_list.pop_front();
		   r = reader.parse(info.c_str(),root);
		   if (!r)
			   continue;
		   songlist.push_back(root["songid"].asString());

	   }
	   //��ȡ������Ϣ
	   GetMusicInfos(redis_engine_,songlist,collect_list);
   }
   return true;
}

bool RedisComm::GetCltAndHateSong(const std::string& uid, 
			std::map<std::string,base::MusicCltHateInfo>& clt_song_map, 
			std::map<std::string,base::MusicCltHateInfo>& hate_song_map){
	std::string os;
	bool r = false;
	char* value = NULL;
	std::list<std::string> song_list;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (!redis_engine_)
		return false;

//���ĸ赥
	os.append("h");
	os.append(uid.c_str());
	os.append("clt");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);

	if (r){
		while(song_list.size()>0){
			std::string content = song_list.front();
			song_list.pop_front();
			base::MusicCltHateInfo mcltinfo;
			mcltinfo.UnserializedJson(content);
			clt_song_map[mcltinfo.songid()] = mcltinfo;
		}
	}

	song_list.clear();
	os.clear();
//�������
	os.append("h");
	os.append(uid.c_str());
	os.append("ht");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);

	if (r){
		while(song_list.size()>0){
			std::string content = song_list.front();
			song_list.pop_front();
			base::MusicCltHateInfo mcltinfo;
			mcltinfo.UnserializedJson(content);
			clt_song_map[mcltinfo.songid()] = mcltinfo;
		}
	}

	return r;

}

bool RedisComm::GetCollectSongs(const std::string& uid, 
					std::map<std::string,base::MusicCltHateInfo>& song_map){
	
	std::string os;
	bool r = false;
	char* value = NULL;
	std::list<std::string> song_list;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (!redis_engine_)
		return false;
	os.append("h");
	os.append(uid.c_str());
	os.append("clt");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);

	if (r){
		while(song_list.size()>0){
			std::string content = song_list.front();
			song_list.pop_front();
			base::MusicCltHateInfo mcltinfo;
			mcltinfo.UnserializedJson(content);
			song_map[mcltinfo.songid()] = mcltinfo;
		}
	}
	//return redis_engine_->GetAllHash(os.c_str(),os.length(),
		 //                            song_map);

	return r;
}

bool RedisComm::GetCollectSongs(const std::string& uid,std::list<std::string>& song_list){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
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

	return r;

}

bool RedisComm::DelCollectSong(const std::string& uid,const std::string& songid){
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

	return  redis_engine_->DelHashElement(os.c_str(),
		                                  songid.c_str(),
										  songid.length());
}


bool RedisComm::GetCollectSong(const std::string& uid,const std::string& songid, 
							   std::string& content){
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
		   content.assign(value,value_len);
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

bool RedisComm::IsCollectSong(const std::string& uid,const std::string& songid){
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

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",songid.c_str());
	}

	return false;
}


bool RedisComm::SetHateSong(const std::string &uid, const std::string &songid,
							const std::string& content){

	//key: huid_ht
	std::string os;
	bool r = false;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;
	//map

	os.append("h");
	os.append(uid.c_str());
	os.append("ht");

	redis_engine_->SetHashElement(os.c_str(),songid.c_str(),songid.length(),
		content.c_str(),content.length());
	return true;
}

bool RedisComm::GetHateSongs(const std::string& uid, 
			std::map<std::string,base::MusicCltHateInfo>& song_map){

	std::string os;
	bool r = false;
	char* value = NULL;
	std::list<std::string> song_list;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (!redis_engine_)
		return false;
	os.append("h");
	os.append(uid.c_str());
	os.append("ht");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);

	if (r){
		while(song_list.size()>0){
			std::string content = song_list.front();
			song_list.pop_front();
			base::MusicCltHateInfo mcltinfo;
			mcltinfo.UnserializedJson(content);
			song_map[mcltinfo.songid()] = mcltinfo;
		}
	}
	//return redis_engine_->GetAllHash(os.c_str(),os.length(),
	//                            song_map);

	return r;
	}

bool RedisComm::DelHateSong(const std::string &uid, const std::string &songid){
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
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
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
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;

	os.append("d");
	os.append(uid.c_str());
	os.append("sl");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);

	return r;
}

void RedisComm::SetMusicAboutUser(const std::string& songid,const std::string& hot_num, 
								  const std::string& cmt_num,const std::string& clt_num){
 
   std::string os;
   std::string key;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
   if (redis_engine_==NULL)
	   return;
   //key: a10000t
   key.append("a");
   key.append(songid.c_str());
   key.append("t");

   //value {"hot":"123","cmt":"231",clt:"2312"}
   os.append("{\"hot\":\"");
   os.append(hot_num.c_str());
   os.append("\",\"cmt\":\"");
   os.append(cmt_num.c_str());
   os.append("\",\"clt\":\"");
   os.append(clt_num.c_str());
   os.append("\"}");

   LOG_DEBUG2("key[%s]",key.c_str());
   LOG_DEBUG2("value[%s]",os.c_str());
   redis_engine_->SetValue(key.c_str(),key.length(),os.c_str(),os.length());
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


int RedisComm::GetHashSize(const std::string& key){
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return 0;
	return redis_engine_->GetHashSize(key.c_str());
}

bool RedisComm::GetUpdateConfig(const std::string& key,std::string& content){
	
	bool r = false;
	char* value;
	size_t value_len = 0;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;
	//LOG_DEBUG2("MgrListenSongsNum key[%s]",os.c_str());
	r = redis_engine_->GetValue(key.c_str(),key.length(),&value,&value_len);
	if (r){
		content.assign(value,value_len);
		if (value){
			delete[] value;
			value = NULL;
		}
	}
	return r;
}
bool RedisComm::MgrListenSongsNum(const std::string& songid,
								  const std::string& last_songid,
								  const std::string& uid){
	//key num_songid:num_99999
	std::string os;
	bool r = false;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;
	os.append("num_");
	os.append(songid.c_str());
	LOG_DEBUG2("MgrListenSongsNum key[%s]",os.c_str());
	r = redis_engine_->SetHashElement(os.c_str(),uid.c_str(),
									  uid.length(),songid.c_str(),
									 songid.length());
	if (atol(last_songid.c_str())>0){
		os.append("num_");
		os.append(last_songid.c_str());
		r = redis_engine_->DelHashElement(os.c_str(),uid.c_str(),
		                              uid.length());
	}
}

void RedisComm::GetMusicInfosV2(std::list<std::string>& songlist,
								std::list<std::string>& songinfolist){
									
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	std::stringstream os;
	int64 total;
	os<<"mget";
	if (songlist.size()==0)
		return;
	while(songlist.size()>0){
		std::string songid = songlist.front();
		os<<" "<<songid.c_str();
		songlist.pop_front();
	}

	GetMusicInfos(redis_engine_,os.str(),songinfolist);
}



void RedisComm::GetMusicInfosV2(std::map<std::string,base::MusicCltHateInfo>& songmap, std::list<std::string>& songinfolist){

#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif

	//redisContext *context = (redisContext *)redis_engine_->GetContext();
	std::stringstream os;
	int64 total;
	bool r = false;
	os<<"mget";
	if (songmap.size()==0)
		return;

	for (std::map<std::string,base::MusicCltHateInfo>::iterator it 
		= songmap.begin();it!=songmap.end();it++){
			std::string songid;
			base::MusicCltHateInfo mclti = it->second;
			os<<" "<<mclti.songid();
	}

	GetMusicInfos(redis_engine_,os.str(),songinfolist);
}

void RedisComm::GetMusicInfosV3(const std::string& type,std::list<int>& random_list, 
								std::list<std::string>& songinfolist){

#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	std::stringstream os;
	int64 total;
	bool r = false;
	std::list<std::string> temp_list;

	//��ȡ������id
	os<<"hmget";
	if (random_list.size()<=0)
		return ;
	os<<" "<<type;

	while(random_list.size()>0){
		int num = random_list.front();
		random_list.pop_front();
		os<<" "<<num;
	}
	LOG_DEBUG2("%s",os.str().c_str());
	
	//��ȡ����
	GetMusicInfos(redis_engine_,os.str(),temp_list);
	os.str("");

	os<<"mget";
	if (temp_list.size()<=0)
		return ;

	while(temp_list.size()>0){
		std::string songid = temp_list.front();
		temp_list.pop_front();
		os<<" "<<songid;
	}
	LOG_DEBUG2("%s",os.str().c_str());


	//��ȡ������Ϣ
	GetMusicInfos(redis_engine_,os.str(),songinfolist);

}

void RedisComm::GetMusicInfosV2(std::map<std::string,std::string>& songmap, 
								std::list<std::string>& songinfolist){
									
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
									
	//redisContext *context = (redisContext *)redis_engine_->GetContext();
	std::stringstream os;
	int64 total;
	bool r = false;
	os<<"mget";
	if (songmap.size()==0)
		return;

	for (std::map<std::string,std::string>::iterator it 
		   = songmap.begin();it!=songmap.end();it++){
		std::string songid = it->second;
		os<<" "<<songid.c_str();
	}

	GetMusicInfos(redis_engine_,os.str(),songinfolist);
	/*LOG_DEBUG2("%s",os.str().c_str());
	if (NULL == context)
		return;
	{
		redisReply *rpl = (redisReply *) redisCommand(context,os.str().c_str());
		base_storage::CommandReply *reply = _CreateReply(rpl);
		freeReplyObject(rpl);
		if (NULL == reply)
			return ;

		//����
		if (base_storage::CommandReply::REPLY_ARRAY == reply->type) {
			base_storage::ArrayReply *arep = 
				static_cast<base_storage::ArrayReply *>(reply);
			base_storage::ArrayReply::value_type &items = arep->value;
			for (base_storage::ArrayReply::iterator it = items.begin(); 
				it != items.end();++it) {
					base_storage::CommandReply *item = (*it);
					if (base_storage::CommandReply::REPLY_STRING == item->type) {
						base_storage::StringReply *srep = static_cast<base_storage::StringReply *>(item);
						songinfolist.push_back(srep->value);
					}
			}
		}
		reply->Release();
	}

	os.str("");*/
}

void RedisComm::GetMusicInfos(base_storage::DictionaryStorageEngine*engine,
							  std::list<std::string>& songlist, 
							  std::list<std::string>& songinfolist){

	//redisContext *context = (redisContext *)engine->GetContext();
	std::stringstream os;
	int64 total;
	os<<"mget";
	while(songlist.size()>0){
	  std::string songid = songlist.front();
	  songlist.pop_front();
	  os<<" "<<songid.c_str();
	}
	GetMusicInfos(engine,os.str(),songinfolist);
	/*LOG_DEBUG2("%s",os.str().c_str());
	if (NULL == context)
	  return;
	{
	  redisReply *rpl = (redisReply *) redisCommand(context,os.str().c_str());
	  base_storage::CommandReply *reply = _CreateReply(rpl);
	  freeReplyObject(rpl);
	  if (NULL == reply)
		  return ;

	  //����
	  if (base_storage::CommandReply::REPLY_ARRAY == reply->type) {
		  base_storage::ArrayReply *arep = 
			  static_cast<base_storage::ArrayReply *>(reply);
		  base_storage::ArrayReply::value_type &items = arep->value;
		  for (base_storage::ArrayReply::iterator it = items.begin(); 
			  it != items.end();++it) {
				  base_storage::CommandReply *item = (*it);
				  if (base_storage::CommandReply::REPLY_STRING == item->type) {
					  base_storage::StringReply *srep = static_cast<base_storage::StringReply *>(item);
					  songinfolist.push_back(srep->value);
				  }
		  }
	  }
	  reply->Release();
	}

	os.str("");
	*/
}

bool RedisComm::GetMusicInfos(base_storage::DictionaryStorageEngine*engine,
                              const std::string& command, 
                              std::list<std::string>& songinfolist){

    redisContext *context = (redisContext *)engine->GetContext();
	LOG_DEBUG2("%s",command.c_str());
	if (NULL == context)
		return false;
	{
		redisReply *rpl = (redisReply *) redisCommand(context,command.c_str());
		base_storage::CommandReply *reply = _CreateReply(rpl);
		freeReplyObject(rpl);
		if (NULL == reply)
			return false;

		//����
		if (base_storage::CommandReply::REPLY_ARRAY == reply->type) {
			base_storage::ArrayReply *arep = 
				static_cast<base_storage::ArrayReply *>(reply);
			base_storage::ArrayReply::value_type &items = arep->value;
			for (base_storage::ArrayReply::iterator it = items.begin(); 
				it != items.end();++it) {
					base_storage::CommandReply *item = (*it);
					if (base_storage::CommandReply::REPLY_STRING == item->type) {
						base_storage::StringReply *srep = static_cast<base_storage::StringReply *>(item);
						songinfolist.push_back(srep->value);
					}
			}
		}
		reply->Release();
	}
	return true;

}

base_storage::CommandReply* RedisComm::_CreateReply(redisReply* reply) {
	using namespace base_storage;
	switch (reply->type) {
	case REDIS_REPLY_ERROR:
		return new ErrorReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_NIL:
		return new CommandReply(CommandReply::REPLY_NIL);
	case REDIS_REPLY_STATUS:
		return new StatusReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_INTEGER:
		return new IntegerReply(reply->integer);
	case REDIS_REPLY_STRING:
		return new StringReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_ARRAY: {
		ArrayReply *rep = new ArrayReply();
		for (size_t i = 0; i < reply->elements; ++i) {
			if (CommandReply *cr = _CreateReply(reply->element[i]))
				rep->value.push_back(cr);
		}
		return rep;
							}
	default:
		break;
	}
	return NULL;
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
								const std::string& songid,
								const std::string& name,
								const std::string& singer,
								const std::string& netstat,
								const std::string& mode,
								const std::string& tid){

	//key cur+uid cur100000
	bool r = false;
	std::string key = "cur";
	std::string value;
	key.append(uid);
	//value {"songid":"10000","state":"1","type":"mm","tid":"1","name":"������","singer":"�Ψ"}
	value.append("{\"songid\":\"");
	value.append(songid);
	value.append("\",\"state\":\"");
	value.append(netstat);
	value.append("\",\"name\":\"");
	value.append(name);
	value.append("\",\"singer\":\"");
	value.append(singer);
	value.append("\",\"type\":\"");
	value.append(mode);
	value.append("\",\"tid\":\"");
	value.append(tid);
	value.append("\"}");
	r = engine_->SetValue(key.c_str(),key.length(),
							value.c_str(),value.length());
	return r;
}

}
