#include "dic_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <assert.h>


namespace robot_storage{

base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;
std::list<base::ConnAddr>  RedisComm::addrlist_;

#if defined (_DIC_POOL_)
threadrw_t* RedisComm::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisComm::dic_conn_pool_;
#endif

AutoDicCommEngine::AutoDicCommEngine()
:engine_(NULL){
#if defined (_DIC_POOL_)
	engine_ = robot_storage::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DIC_POOL_)
	robot_storage::RedisComm::RedisConnectionPush(engine_);
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
	robot_logic::WLockGd lk(dic_pool_lock_);
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
	robot_logic::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	robot_logic::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif


bool RedisComm::AddNewMusicInfo(const std::string& songid,const std::string& album,
				const std::string& title,const std::string& pubtime,const std::string& artist,
				const std::string& pic){
#if defined (_DIC_POOL_)
	AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
	if (redis_engine_==NULL)
		return false;

	/*
	 * {\"id\":\"121232\",
	 * \"sid\":\"158700\",
	 * \"ssid\":\"78e7\",
	 * \"album_title\":\"TGl2ZSBQcml2YXRlIEFjb3VzdGkuLi4=\",
	 * \"titile\":\"VGVlbmFnZSBBbmdzdA==\",
	 * \"pub_time\":\"2004\",
	 * \"artist\":\"UGxhY2Vibw==\",
	 * \"pic_url\":\"http://img1.douban.com/lpic/s3098363.jpg\"}
	 * */
	result["id"] = songid;
	result["sid"] = "0";
	result["ssix"] = "0";
	result["album_title"] = album;
	result["titile"] = title;
	result["pubtime"] = pubtime;
	result["artist"] = artist;
	result["pic_url"] = pic;
	reponse = wr.write(result);
	redis_engine_->AddValue(songid.c_str(),songid.length(),
				reponse.c_str(),reponse.length());
	return true;
}

void RedisComm::GetBachMusicInfos(std::list<int64>& songid_list,std::list<std::string>& songinfolist){

#if defined (_DIC_POOL_)
	AutoDicCommEngine auto_engine;
	base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	std::stringstream os;
	int64 total;
	os<<"mget";
	while(songid_list.size()>0){
		int64 songid = songid_list.front();
		songid_list.pop_front();
		os<<" "<<songid;
	}
	GetMusicInfos(redis_engine_,os.str(),songinfolist);
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

		//
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

base_storage::DictionaryStorageEngine* RedisComm::GetConnection(){

	try{
		base_storage::DictionaryStorageEngine* engine =
				robot_logic::ThreadKey::GetStorageDicConn();
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

		robot_logic::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}




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

bool MemComm::GetUserToken(const std::string& platform_id,
		          const std::string& uid,std::string& token){
    bool r = false;
	std::string key;
    key.append(platform_id);
    key.append("_");
    key.append(uid);
	size_t value_len = 0;
	char* value = NULL;
	r = engine_->GetValue(key.c_str(),key.length(),&value,&value_len);
	if(r){
		token.assign(value,value_len);
		return true;
	}
	return false;

}
}
