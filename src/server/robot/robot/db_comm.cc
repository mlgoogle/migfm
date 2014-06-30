#include "db_comm.h"
#include "base/thread_handler.h"
#include "base/logic_comm.h"
#include "basic/basictypes.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace robot_storage{

#if defined (_DB_POOL_)
threadrw_t* DBComm::db_pool_lock_;
std::list<base_storage::DBStorageEngine*>  DBComm::db_conn_pool_;
#endif

std::list<base::ConnAddr> DBComm::addrlist_;


AutoDBCommEngine::AutoDBCommEngine()
:engine_(NULL){
#if defined (_DB_POOL_)
	engine_ = robot_storage::DBComm::DBConnectionPop();
#endif
}

AutoDBCommEngine::~AutoDBCommEngine(){
#if defined (_DB_POOL_)
	robot_storage::DBComm::DBConnectionPush(engine_);
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
//获取用户的坐标
bool DBComm::GetUserLbsPos(const int64 uid,double& latitude,double& longitude){
#if defined (_DB_POOL_)
	AutoDBCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	int num;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}


	//call migfm.proc_GetLbsAboutInfos(10108);
	os<<"call migfm.proc_GetLbsAboutInfos("<<uid<<");";

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
			longitude = atof(rows[5]);
		}
		return true;
	}
	latitude = 0;
	longitude = 0;
	return false;
}

//更新机器人坐标
bool DBComm::UpdateRobotLbsPos(const int64 uid,double latitude,double longitude){
#if defined (_DB_POOL_)
	AutoDBCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif

	char s_latitude[256];
	char s_longitude[256];
	std::stringstream os;
	bool  r = false;
	snprintf(s_latitude, arraysize(s_latitude),
		"%lf", latitude);
	snprintf(s_longitude, arraysize(s_longitude),
		"%lf",longitude);

	LOG_DEBUG2("latitude %s longitude %s",s_latitude,s_longitude);

	os<<"call proc_RecordRobotLbsPos(\'"
		<<uid
		<<"\',"<<s_latitude
		<<","<<s_longitude
		<<"); ";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());
	return true;
}


bool DBComm::GetRobotInfos(const int from,const int count,RobotInfosMap& robot_infos){
#if defined (_DB_POOL_)
	AutoDBCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	int num;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}


	//call migfm.proc_GetRobotsInfo(100,0);
	os<<"call migfm.proc_GetRobotsInfo("<<count<<","<<from<<");";

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
			int64 uid = atoll(rows[0]);
			std::string nickname = rows[1];
			int32 sex = atoi(rows[2]);
			std::string head_url = rows[3];
			robot_base::RobotBasicInfo robot_info(uid,sex,0,0,0,nickname,head_url);
			robot_infos[uid] = robot_info;
		}
		return true;
	}
	return false;
}

//获取当前机器人试听的歌曲
bool DBComm::GetRobotLoginListenSong(const int64 uid,int64& songid){
#if defined (_DB_POOL_)
	AutoDBCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	int num;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}
	//call migfm.proc_GetRobotLoginListenSong(100008);
	os<<"call migfm.proc_GetRobotLoginListenSong("<<uid<<");";
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
			songid = atoll(rows[0]);
		}
		return true;
	}
	return false;

}

bool DBComm::GetChannelInfos(std::list<int64>& list){
	std::stringstream os;
	bool r = false;
	int num = 0;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	base_storage::db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	os<<"select id from migfm_channel;";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error ");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			int64 channel_id = atol(rows[0]);
			list.push_back(channel_id);
		}
	}
	return true;
}

bool DBComm::GetMoodInfos(std::list<int64> &list){
	std::stringstream os;
	bool r = false;
	int num = 0;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	base_storage::db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	os<<"select id from migfm_mood;";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error ");
		return r;
	}
	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			int channel_id = atol(rows[0]);
			list.push_back(channel_id);
		}
		return true;
	}
	return false;
}

bool DBComm::GetSceneInfos(std::list<int64> &list){
	std::stringstream os;
	bool r = false;
	int num = 0;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	base_storage::db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	os<<"select id from migfm_scene;";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error ");
		return r;
	}
	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			int channel_id = atol(rows[0]);
			list.push_back(channel_id);
		}
		return true;
	}
	return false;
}

bool DBComm::GetUsersLBSPos(std::list<robot_base::UserLbsInfo>& user_lbs_list){
	std::stringstream os;
	bool r = false;
	int num = 0;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	base_storage::db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	os<<"call migfm.proc_GetUserLBSPos(0,1000)";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error ");
		return r;
	}
	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			int64 uid = atoll(rows[0]);
			std::string latitude = rows[1];
			std::string longitude = rows[2];
			robot_base::UserLbsInfo lbs_info(uid,latitude,longitude);
			//int channel_id = atol(rows[0]);
			user_lbs_list.push_back(lbs_info);
		}
		return true;
	}
	return false;
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


}
