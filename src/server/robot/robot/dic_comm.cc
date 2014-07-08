#include "dic_comm.h"
#include "base/logic_comm.h"
#include <sstream>

namespace robot_storage{



base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;
std::list<base::ConnAddr>  RedisComm::addrlist_;


bool MemComm::Init(std::list<base::ConnAddr>& addrlist){
	bool r = false;
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	if (engine_==NULL)
		return false;
	r = engine_->Connections(addrlist);
	return r;
}

void MemComm::Dest(){
	if (engine_){
		delete engine_;
		engine_ = NULL;
	}
}



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


void RedisComm::Init(std::list<base::ConnAddr>& addrlist,
		const int32 dic_conn_num){
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
		bool r =  engine->Connections(addrlist_);
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

bool RedisComm::GetMusicInfos(const int64 songid,std::string& musicinfo) {
	char* value;
	size_t value_len = 0;
	char key[256] = {0};
	size_t key_length = strlen(key);
	snprintf(key, arraysize(key), "%lld", songid);
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;
	bool r = redis_engine_->GetValue(key,key_length,
		&value,&value_len);
	if (r){
		musicinfo.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",key);
	}
	return true;
}

bool RedisComm::GetBatchMusicInfos(const std::string& type,std::list<int64>& random_list,
        std::list<std::string>& batchinfolist){
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	std::stringstream os;
	int64 total;
	bool r = false;
	std::list<std::string> temp_list;

	os<<"hmget";
	if (random_list.size()<=0)
		return false;
	os<<" "<<type;

	while(random_list.size()>0){
		int num = random_list.front();
		random_list.pop_front();
		os<<" "<<num;
	}
	LOG_DEBUG2("%s",os.str().c_str());
	//获取到相对应音乐id
	r = GetBatchInfos(redis_engine_,os.str(),temp_list);
	os.str("");
	if(!r)
		return false;

	os<<"mget";
	if (temp_list.size()<=0)
		return false;

	while(temp_list.size()>0){
		std::string songid = temp_list.front();
		temp_list.pop_front();
		os<<" "<<songid;
	}
	LOG_DEBUG2("%s",os.str().c_str());


	//获取音乐信息
	r = GetBatchInfos(redis_engine_,os.str(),batchinfolist);
	if(!r)
		return false;

	return r;
}



bool RedisComm::GetBatchInfos(base_storage::DictionaryStorageEngine*engine,
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

int RedisComm::GetHashSize(const std::string& key){
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return 0;
	return redis_engine_->GetHashSize(key.c_str());
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

}
