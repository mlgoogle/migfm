#include "dic_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <assert.h>


namespace manager_storage{

static const char *HKEY_PUSH_CFG_DEV_TOK = "soc:push.cfg:dev.tok";
static const char *HKEY_PUSH_CFG_IS_RECV = "soc:push.cfg:is.recv";
static const char *HKEY_PUSH_CFG_BTIME = "soc:push.cfg:btime";
static const char *HKEY_PUSH_CFG_ETIME = "soc:push.cfg:etime";
static const char *KEY_PUSH_MSG_STAGE = "soc:%lld:push.msg";
static const char *KEY_MSG_ID_GEN = "chat:msg.id:next";
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




#if defined (_DIC_POOL_)
threadrw_t* RedisComm::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisComm::dic_conn_pool_;
#endif


std::list<base::ConnAddr>  RedisComm::addrlist_;

AutoDicCommEngine::AutoDicCommEngine()
:engine_(NULL){
#if defined (_DB_POOL_)
	engine_ = manager_storage::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DB_POOL_)
	manager_storage::RedisComm::RedisConnectionPush(engine_);
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
	manager_logic::WLockGd lk(dic_pool_lock_);
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
	manager_logic::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	manager_logic::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif

base_storage::DictionaryStorageEngine* RedisComm::GetConnection() {
	try{
		base_storage::DictionaryStorageEngine* engine =
				manager_logic::ThreadKey::GetStorageDicConn();
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
		manager_logic::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool RedisComm::GetUserPushConfig(int64 uid, std::string& device_token,
		bool& is_receive, unsigned& begin_time, unsigned& end_time) {
	device_token.clear();
	is_receive = true;
	begin_time = 0;
	end_time = 0;

#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis  = auto_engine.GetDicEngine();
#endif

	if (NULL == redis)
		return false;

	char field[256] = {0};
	char *val = NULL; //[1024] = {0};
	snprintf(field, arraysize(field), "%lld", uid);
	size_t field_len = strlen(field);
	size_t val_len = 0;
	bool ret = false;

	ret = redis->GetHashElement(HKEY_PUSH_CFG_DEV_TOK, field, field_len, &val, &val_len);
	if (ret && val) {
		device_token.assign(val, val_len);
		free(val);
	} else {
		return false;
	}

	val = NULL;
	val_len = 0;
	ret = redis->GetHashElement(HKEY_PUSH_CFG_IS_RECV, field, field_len, &val, &val_len);
	if (ret && val) {
		is_receive = atoi(val)!=0 ? true : false;
		free(val);
	} else {
		is_receive = true;
	}

	val = NULL;
	val_len = 0;
	redis->GetHashElement(HKEY_PUSH_CFG_BTIME, field, field_len, &val, &val_len);
	if (ret && val) {
		begin_time = atoi(val);
		free(val);
	} else {
		begin_time = DEFAULT_BEGIN_TIME;
	}

	val = NULL;
	val_len = 0;
	redis->GetHashElement(HKEY_PUSH_CFG_ETIME, field, field_len, &val, &val_len);
	if (ret && val) {
		end_time = atoi(val);
		free(val);
	} else {
		end_time = DEFAULT_END_TIME;
	}

	return true;
}

bool RedisComm::StagePushMsg(int64 uid, int64 msg_id, const std::string& msg) {
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis  = auto_engine.GetDicEngine();
#endif

	if (NULL == redis)
		return false;

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_PUSH_MSG_STAGE, uid);
	LOG_DEBUG2("key [%s] msg [%s]",key,msg.c_str());
	return redis->AddListElement(key, strlen(key),
		                         msg.c_str(),
								 msg.length(),1);
}


bool RedisComm::GenaratePushMsgID(int64& msg_id) {
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis  = auto_engine.GetDicEngine();
#endif

	if (NULL == redis)
		return false;

	//char key[256] = {0};
	//snprintf(key, KEY_MSG_ID_GEN, uid);
	return redis->IncDecValue(KEY_MSG_ID_GEN, strlen(KEY_MSG_ID_GEN), 1, msg_id);
}

}
