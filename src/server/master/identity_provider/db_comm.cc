#include "db_comm.h"
#include "logic_comm.h"
#include "thread_handler.h"
#include "basic/basic_info.h"
#include "basic/base64.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace storage{

#if defined (_STORAGE_POOL_)
base_storage::DBStorageEngine** DBComm::db_conn_pool_;
int32 DBComm::db_conn_num_;
threadrw_t* DBComm::db_pool_lock_;
#endif


std::list<base::ConnAddr> DBComm::addrlist_;

void DBComm::Init(std::list<base::ConnAddr>& addrlist,
				  const int32 db_conn_num/* = 10*/){
	addrlist_ = addrlist;
#if defined (_DB_POOL_)	
	db_conn_num_ = db_conn_num;
	db_conn_pool_ = new (std::nothrow) base_storage::DBStorageEngine* [db_conn_num_];
	if (db_conn_pool_==NULL){
		LOG_ERROR2("db_conn_pool error[%s]",sterrno(errno));
		return;
	}
	InitThreadrw(db_pool_lock_);
	for (int i = 0;i<=db_conn_num_;i++){
		db_conn_pool_[i] = CreateConnection();
	}
#endif
}

#if defined (_DB_POOL_)
base_storage::DBStorageEngine* DBComm::CreateConnection(){

}

void DBComm::DBConnectionPush(base_storage::DBStorageEngine* db){
	WLockGd lk(db_pool_lock_);
	db_conn_pool_[++db_conn_num_] = db;

	assert(db_conn_num_<=10);
	LOG_DEBUG2("db_conn_pool num[%d]",num_);
}

base_storage::DBStorageEngine* DBComm::DBConnectionPop(){

}

#endif
void DBComm::Dest(){
#if defined (_DB_POOL_)
    for (int i = 0;i<db_conn_num_;i++){
		base_storage::DBStorageEngine* engine = db_conn_pool_[i];
		if (engine){
			engine->Release();
			delete engine;
			engine = NULL;
		}
    }
	DeinitThreadrw(db_pool_lock_);

#endif
}

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
		bool r = false;
		base_storage::DBStorageEngine* engine = sso_logic::ThreadKey::GetStorageDBConn();
		if (engine){
			if (!engine->CheckConnect()){
				LOG_ERROR("Database %s connection was broken");
				engine->Release();
				if (engine){
					delete engine;
					engine = NULL;
				}
			}else
				return engine;
		}

		engine = base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		r = engine->Connections(addrlist_);
		if (!r)
			return NULL;
		sso_logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}
}