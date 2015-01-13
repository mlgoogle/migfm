#include "db_comm.h"
#include "logic/logic_comm.h"
#include "base/logic_comm.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace chat_storage{

#if defined (_DB_POOL_)
threadrw_t* DBComm::db_pool_lock_;
std::list<base_storage::DBStorageEngine*>  DBComm::db_conn_pool_;
#endif

std::list<base::ConnAddr> DBComm::addrlist_;


AutoDBCommEngine::AutoDBCommEngine()
:engine_(NULL){
#if defined (_DB_POOL_)
	engine_ = chat_storage::DBComm::DBConnectionPop();
#endif
}

AutoDBCommEngine::~AutoDBCommEngine(){
#if defined (_DB_POOL_)
	chat_storage::DBComm::DBConnectionPush(engine_);
#endif
}



void DBComm::Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num/* = 5*/){
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
	logic::WLockGd lk(db_pool_lock_);
	db_conn_pool_.push_back(engine);
}

base_storage::DBStorageEngine* DBComm::DBConnectionPop(){
	if(db_conn_pool_.size()<=0)
		return NULL;
	logic::WLockGd lk(db_pool_lock_);
    base_storage::DBStorageEngine* engine = db_conn_pool_.front();
    db_conn_pool_.pop_front();
    return engine;
}

#endif


void DBComm::Dest(){
#if defined (_DB_POOL_)
	logic::WLockGd lk(db_pool_lock_);
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

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
		bool r = false;
		base_storage::DBStorageEngine* engine = logic::ThreadKey::GetStorageDBConn();
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
		logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool DBComm::RecordUserMessageList(const int32 type,const int64 send_uid,
								const int64 to_uid,const double distance,
								const std::string& message){
#if defined (_DB_POOL_)
	AutoDBCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}
	char s_distance[256];
	snprintf(s_distance, arraysize(s_distance),
			"%lf", distance);

	// call proc_RecordMessageList(1,'来一发',10108,10158,'0','123232.09999')
	os	<< "call proc_RecordMessageList("
		<<type<<",\'"<<message<<"\',"<<send_uid
		<<","<<to_uid<<",\'0\',\'"<<s_distance<<"\')";

	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}
	return true;
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
	//call proc_ChatRecordMessage(10001,12321323,10013,10014,'o','2014-03-10')
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


bool DBComm::GetUserLBSPos(const int64& uid,double& latitude,double& longitude){
    // call proc_ChatGetUserInfo(platform_id,user_id)
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	bool r =false;
	MYSQL_ROW rows;
	int num;;
	if (engine==NULL){
	 LOG_ERROR("GetConnection Error");
	 return false;
	}

	//call proc_GetLbsAboutInfos(10108)
	os<<"call proc_GetLbsAboutInfos("<<uid<<");";

	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}
	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			latitude = atof(rows[4]);
			longitude =  atof(rows[5]);
		}
		return true;
	}
	return false;
}


bool DBComm::GetRobotsUserInfo(const int64 platform_id,int64 user_id,
							 chat_base::UserInfo& userinfo){
//call migfm.proc_GetRobotInfo(169111)
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	bool r =false;
	MYSQL_ROW rows;
	int num;;
	if (engine==NULL){
	 LOG_ERROR("GetConnection Error");
	 return false;
	}

	os<<"call proc_GetRobotInfo("<<platform_id
		<<","<<user_id<<");";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}
	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			//song_url = rows[0];
			int64 nicknumber = atoll(rows[0]);
			std::string nickname = rows[1];
			std::string sex = rows[2];
			std::string head_url = rows[3];
			chat_base::UserInfo current_userinfo(platform_id,user_id,
				                            nicknumber,nickname,
									        head_url);
			userinfo = current_userinfo;
		}
		return true;
	}
	return false;
}

bool DBComm::GetUserInfo(const int64 platform_id,int64 user_id, 
						 chat_base::UserInfo& userinfo){
    
     // call proc_ChatGetUserInfo(platform_id,user_id)
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	bool r =false;
	MYSQL_ROW rows;
	int num;;
	if (engine==NULL){
	 LOG_ERROR("GetConnection Error");
	 return false;
	}

	os<<"call proc_ChatGetUserInfo("<<platform_id
		<<","<<user_id<<");";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}
	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			//song_url = rows[0];
			int64 nicknumber = atoll(rows[0]);
			std::string nickname = rows[1];
			std::string head_url = rows[2];
			chat_base::UserInfo current_userinfo(platform_id,user_id,
				                            nicknumber,nickname,
									        head_url);
			userinfo = current_userinfo;
		}
		return true;
	}
	return false;
}

bool DBComm::RecordChatMessage(const int32 type,chat_base::MessageInfos& message){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		bool r =false;
		MYSQL_ROW rows;
		if (engine==NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}
		if(type == ALONE_CHAT)
			os<<"call proc_V2RecordAloneMessage("<<message.platform()<<","
				<<message.msgid()<<","
				<<message.fid()<<",\'"<<message.nickname()<<"\',"<<message.oppid()<<",\'"
				<<message.lasttime()<<"\',\'"<<message.message()<<"\',\'"
				<<message.head()<<"\');";
		else
			os<<"call proc_V2RecordGroupMessage("<<message.platform()<<","
				<<message.oppid()<<","<<message.msgid()<<","
				<<message.fid()<<",\'"<<message.nickname()<<"\',\'"
				<<message.lasttime()<<"\',\'"<<message.message()<<"\',\'"
				<<message.head()<<"\');";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR("exec sql error");
		}
		return r;
}

bool DBComm::GetDimensionGroup(chat_logic::PlatformChatCacheManager* cache){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		bool r =false;
		MYSQL_ROW rows;
		int num;;
		if (engine==NULL){
		 LOG_ERROR("GetConnection Error");
		 return false;
		}

		os<<"call proc_V2GetDimensionGroup();";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR("exec sql error");
			return false;
		}
		num = engine->RecordCount();
		if(num>0){
			while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
				int64 groupid = 0;
				int64 nicknumber = 0;
				std::string group_name;
				std::string head;
				if(rows[0]!=NULL)
					groupid = atoll(rows[0]);
				if(rows[1]!=NULL)
					nicknumber = atoll(rows[1]);
				if(rows[2]!=NULL)
					group_name =rows[2];
				chat_base::GroupInfo groupinfo(10000,groupid,DIMENSION_GROUP_CHAT,
						nicknumber,nicknumber,group_name,head);
				cache->AddGroupInfos(10000,groupid,groupinfo);
			}
			return true;
		}
		return false;
}
}
