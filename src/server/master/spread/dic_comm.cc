#include "dic_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <assert.h>


namespace spread_storage{

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
	engine_ = spread_storage::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DB_POOL_)
	spread_storage::RedisComm::RedisConnectionPush(engine_);
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
	spread_logic::WLockGd lk(dic_pool_lock_);
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
	spread_logic::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	spread_logic::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif

base_storage::DictionaryStorageEngine* RedisComm::GetConnection() {
	try{
		base_storage::DictionaryStorageEngine* engine =
				spread_logic::ThreadKey::GetStorageDicConn();
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
		spread_logic::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

}
