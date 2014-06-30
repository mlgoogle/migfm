#include "db_comm.h"
#include "thread_handler.h"
#include "basic/basic_info.h"
#include "basic/base64.h"
#include "storage/storage.h"

#include <mysql.h>
#include <sstream>

namespace manager_storage{

#if defined (_DB_POOL_)
threadrw_t* DBComm::db_pool_lock_;
std::list<base_storage::DBStorageEngine*>  DBComm::db_conn_pool_;
#endif

std::list<base::ConnAddr> DBComm::addrlist_;


AutoDBCommEngine::AutoDBCommEngine()
:engine_(NULL){
#if defined (_DB_POOL_)
	engine_ = manager_storage::DBComm::DBConnectionPop();
#endif
}

AutoDBCommEngine::~AutoDBCommEngine(){
#if defined (_DB_POOL_)
	manager_storage::DBComm::DBConnectionPush(engine_);
#endif
}

void DBComm::Init(std::list<base::ConnAddr>& addrlist,
				  const int32 db_conn_num/* = 5*/){
	addrlist_ = addrlist;

#if defined (_DB_POOL_)
	bool r =false;
	InitThreadrw(&db_pool_lock_);
	for (int i = 0; i<db_conn_num;i++){
		base_storage::DBStorageEngine* engine  =
				base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
		if (engine==NULL){
			assert(0);
			LOG_ERROR("create db conntion error");
			continue;
		}

		r = engine->Connections(addrlist_);
		if (!r){
			assert(0);
			LOG_ERROR("db conntion error");
			continue;
		}

		db_conn_pool_.push_back(engine);

	}

#endif
}

#if defined (_DB_POOL_)

void DBComm::DBConnectionPush(base_storage::DBStorageEngine* engine){
	manager_logic::WLockGd lk(db_pool_lock_);
	db_conn_pool_.push_back(engine);
}

base_storage::DBStorageEngine* DBComm::DBConnectionPop(){
	if(db_conn_pool_.size()<=0)
		return NULL;
	manager_logic::WLockGd lk(db_pool_lock_);
    base_storage::DBStorageEngine* engine = db_conn_pool_.front();
    db_conn_pool_.pop_front();
    return engine;
}

#endif


void DBComm::Dest(){
#if defined (_DB_POOL_)
	manager_logic::WLockGd lk(db_pool_lock_);
	while(db_conn_pool_.size()>0){
		base_storage::DBStorageEngine* engine = db_conn_pool_.front();
		db_conn_pool_.pop_front();
		if(engine){
			engine->Release();
			delete engine;
			engine =NULL;
		}
	}
	DeinitThreadrw(db_pool_lock_);

#endif
}


bool  DBComm::GetPushMessage(std::string& summary,std::string& message){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		bool r = false;
		MYSQL_ROW rows;
		int return_code = 0;
		if(engine == NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}
		//call migfm.proc_GetPushMessage();
		os<<"call migfm.proc_GetPushMessage()";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}

		int num = engine->RecordCount();
		if (num>0){
			while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
				summary = rows[0];
				message = rows[1];
			}
			return true;
		}
		return false;

}

//超过多少时间未登录的用户
bool DBComm::GetOverTimeNoLogin(const int64 from,const int64 count,std::list<int64>& userinfo){

#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		bool r = false;
		int64 uid;
		MYSQL_ROW rows;
		int return_code = 0;
		if(engine == NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}
		os<<"call proc_GetOverTimeUser("<<from<<","<<count<<")";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}

		int num = engine->RecordCount();
		if (num>0){
			while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
				uid = atoll(rows[0]);
				userinfo.push_back(uid);
			}
			return true;
		}
		return false;
}


bool DBComm::GetUsersInfo(const int64 from,const int64 count,std::list<int64>& userinfo){

#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		bool r = false;
		int64 uid;
		MYSQL_ROW rows;
		int return_code = 0;
		if(engine == NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}
		//call migfm.proc_GetPushMessage();
		os<<"call proc_GetAllUserInfos("<<from<<","<<count<<")";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}

		int num = engine->RecordCount();
		if (num>0){
			while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
				uid = atoll(rows[0]);
				userinfo.push_back(uid);
			}
			return true;
		}
		return false;
}

bool DBComm::RecordMessage(const int64 platform_id,const int64 fid,const int64 tid,
					const int64 msg_id,const std::string& message,
					const std::string& current_time){

#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	std::string sql;
	bool r = false;
	MYSQL_ROW rows;
	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}
	/*char* temp_message = new char[message.length()];
	memset(temp_message,'\0',message.length());
	sprintf(temp_message,"%s",message.c_str());
    */
	LOG_DEBUG2("current_time = %s msg_id = %lld send_id = %lld recv_id = %lld content = %s",
			current_time.c_str(),msg_id,fid,tid,message.c_str());
	//call migfm.proc_ChatRecordMessage(10001,12321323,10013,10014,'o','2014-03-10')
	os<<"call proc_ChatRecordMessage("<<platform_id<<","<<msg_id<<","
			<<fid<<","<<tid<<",'"<<current_time<<"','"<<message<<"');";
	sql = os. str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}
	/*if(temp_message){
		delete [] temp_message;
		temp_message = NULL;
	}*/
	return r;

}

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
		bool r = false;
		base_storage::DBStorageEngine* engine = manager_logic::ThreadKey::GetStorageDBConn();
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
		manager_logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

}
