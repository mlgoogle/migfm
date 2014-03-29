#include "db_comm.h"
#include "logic_comm.h"
#include "thread_handler.h"
#include "basic/basic_util.h"
#include <mysql.h>
#include <sstream>

#define USERINFOS "migfm_user_infos"

namespace storage {

#if defined (_DB_POOL_)
threadrw_t* DBComm::db_pool_lock_;
std::list<base_storage::DBStorageEngine*>  DBComm::db_conn_pool_;
#endif

#if defined (_DB_SINGLE_)
threadrw_t*                                DBComm::db_single_lock_;
base_storage::DBStorageEngine*             DBComm::db_conn_single_;
#endif


std::list<base::ConnAddr> DBComm::addrlist_;


AutoDBCommEngine::AutoDBCommEngine()
:engine_(NULL){
#if defined (_DB_POOL_)
	engine_ = storage::DBComm::DBConnectionPop();
#endif
}

AutoDBCommEngine::~AutoDBCommEngine(){
#if defined (_DB_POOL_)
	storage::DBComm::DBConnectionPush(engine_);
#endif
}

void DBComm::Init(std::list<base::ConnAddr>& addrlist,
				  const int32 db_conn_num/* = 10*/){
	addrlist_ = addrlist;

#if defined (_DB_POOL_)
	bool r =false;
	InitThreadrw(&db_pool_lock_);
	for (int i = 0; i<=db_conn_num;i++){
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

#if defined (_DB_SINGLE_)
	bool r =false;
	InitThreadrw(&db_single_lock_);
	db_conn_single_ =
			base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
	if (db_conn_single_==NULL){
		assert(0);
		LOG_ERROR("create db conntion error");
		return ;
	}

	r = db_conn_single_->Connections(addrlist_);
	if (!r){
		assert(0);
		LOG_ERROR("db conntion error");
		return ;
	}
#endif
}

#if defined (_DB_POOL_)

void DBComm::DBConnectionPush(base_storage::DBStorageEngine* engine){
	mig_lbs::WLockGd lk(db_pool_lock_);
	db_conn_pool_.push_back(engine);
}

base_storage::DBStorageEngine* DBComm::DBConnectionPop(){
	if(db_conn_pool_.size()<=0)
		return NULL;
	mig_lbs::WLockGd lk(db_pool_lock_);
    base_storage::DBStorageEngine* engine = db_conn_pool_.front();
    db_conn_pool_.pop_front();
    return engine;
}

#endif


void DBComm::Dest(){
#if defined (_DB_POOL_)
	mig_lbs::WLockGd lk(db_pool_lock_);
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

#if defined (_DB_SINGLE_)
	mig_lbs::WLockGd lk(db_single_lock_);
	if(db_conn_single_){
		delete db_conn_single_;
		db_conn_single_ = NULL;
	}
	DeinitThreadrw(db_single_lock_);
#endif
}

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
		bool r = false;
		base_storage::DBStorageEngine* engine = mig_lbs::ThreadKey::GetStorageDBConn();
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
		mig_lbs::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool DBComm::GetUserInfos(const std::string& uid,
						  std::string& nickname,std::string& gender,
						  std::string& pic) {
	nickname.clear();
	gender.clear();

#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}
	os	<< "select nickname,sex,head from " << USERINFOS
		<< " where usrid=\'" << uid.c_str() << "\';";
	const char* sql = os.str().c_str();
	LOG_DEBUG2("[%s]",os.str().c_str());
	r = engine->SQLExec(os.str().c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	int32 num = engine->RecordCount();
	if (num > 0) {
		if (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
			nickname = rows[0];
			gender = rows[1];
			pic = rows[2];
		}
		return true;
	}
	return false;
}

bool DBComm::GetLBSAboutInfos(const std::string& uid,std::string& sex,std::string& nickname,
							  std::string& head,std::string& birthday,
							  double& latitude,double& longitude){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
		std::stringstream os;
		bool r = false;
		MYSQL_ROW rows;
		if (engine == NULL) {
			LOG_ERROR("GetConnection Error");
			return false;
		}
		//call migfm.proc_GetLbsAboutInfos(10108);
		os<<"call migfm.proc_GetLbsAboutInfos("
			<<uid<<")";

		const char* sql = os.str().c_str();
		LOG_DEBUG2("[%s]",os.str().c_str());
		r = engine->SQLExec(os.str().c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}
		int32 num = engine->RecordCount();
		if (num > 0) {
			if (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
				sex = rows[0];
				nickname = rows[1];
				head = rows[2];
				birthday = rows[3];
				latitude = atof(rows[4]);
				longitude = atof(rows[5]);
			}
			return true;
		}
		return false;

}

bool DBComm::GetUserLbsPos(const int64 src_uid,double& latitude, 
						   double& longitude){
							   
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}
	os<<"select latitude,longitude from migfm_lbs_pos where uid = "
		<<src_uid;

	const char* sql = os.str().c_str();
	LOG_DEBUG2("[%s]",os.str().c_str());
	r = engine->SQLExec(os.str().c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	int32 num = engine->RecordCount();
	if (num > 0) {
		if (rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
			latitude = atof(rows[0]);
			longitude = atof(rows[1]);
		}
		return true;
	}
	return false;
}


bool DBComm::GetSameMusic(Json::Value& users,const int64 src_uid,const double latitude,
                          const double longitude){
    std::stringstream os;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
    bool r = false;
	MYSQL_ROW rows;
	int num;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}

	//获取用户信息
	//call migfm.proc_GetSameMusicUser(10108)
	os<<"call proc_GetSameMusicUser("<<src_uid<<")";
	r = engine->SQLExec(os.str().c_str());
	LOG_DEBUG2("%s",os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			Json::Value val;
			val["userinfo"]["userid"] = rows[1];
			val["userinfo"]["birthday"] = rows[2];
			val["userinfo"]["head"] = rows[3];
			val["userinfo"]["latitude"] = atof(rows[4]);
			val["userinfo"]["longitude"] = atof(rows[5]);
			val["userinfo"]["nickname"] = rows[6];
			val["userinfo"]["sex"] = rows[7];
			val["userinfo"]["userid"] = rows[8];
			val["music"]["id"] = rows[9];
			val["userinfo"]["distance"]
			  =  base::BasicUtil::CalcGEODistance(latitude,longitude,
					  atof(rows[4]),atof(rows[5]));
			users.append(val);
		}
		return true;
	}
	return false;
}

bool DBComm::UpDateUserLbsPos(Json::Value& users,const int64 src_uid){

#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}

	for (Json::Value::iterator it = users.begin();
		it != users.end(); ++it){
			Json::Value &item = *it;
		//call proc_RecordUserLbsPos(10108,30.292207031178,120.0855621569,6000);
			//double latitude = item["userinfo"]["latitude"].asDouble();
			//double longitude = item["userinfo"]["longitude"].asDouble();
			char latitude[256];
			char longitude[256];
			snprintf(latitude, arraysize(latitude), 
				"%lf", item["userinfo"]["latitude"].asDouble());
			snprintf(longitude, arraysize(longitude), 
				"%lf", item["userinfo"]["longitude"].asDouble());

			LOG_DEBUG2("latitude %s longitude %s",latitude,longitude);

			os<<"call proc_RecordUserLbsPos("
				<<item["userinfo"]["userid"].asString().c_str()
				<<","<<latitude
				<<","<<longitude
				<<"); ";
			std::string sql = os.str();
			LOG_DEBUG2("[%s]", sql.c_str());
			r = engine->SQLExec(sql.c_str());
			os.str("");
			if (!r) {
				LOG_ERROR2("exec sql error");
				return false;
			}
	}

	/*std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	*/
    return true;
}


bool DBComm::GetMusicUrl(const std::string &song_id, std::string &hq_url, std::string &song_url){
	std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
	if (engine==NULL){
		LOG_ERROR("engine error");
		return true;
	}
	base_storage::db_row_t* db_rows;
	int num;
	MYSQL_ROW rows = NULL;
	os<<"select song_hifi_url,song_url from migfm_music_url where song_id =\'"
		<<song_id.c_str()<<"\';";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			if (rows[0] ==NULL)
				hq_url = DEFAULT_URL;
			else
				hq_url = rows[0];

			if (rows[1] ==NULL){
				if (rows[0]!=NULL)
					song_url = hq_url;
				else
					song_url = DEFAULT_URL;
			}
			else
				song_url = rows[1];
		}
		song_url = hq_url;
		return true;
	}else{
		hq_url = song_url = DEFAULT_URL;
		return false;
	}
	return true;
}

bool DBComm::GetMusicAboutInfo(const std::string& song_id,std::string& hq_url,std::string& song_url,
		std::string& clt_num,std::string& cmt_num,std::string& hot_num){
	std::stringstream os;
	bool r = false;
	int num;
	MYSQL_ROW rows = NULL;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
	if (engine==NULL){
		LOG_ERROR("engine error");
		return true;
	}
	//call migfm.proc_GetMusicAboutInfo(325636)
	os<<"call proc_GetMusicAboutInfo("<<song_id<<");";
	//os<<"select clt_num,cmt_num ,hot_num from migfm_music_about where song_id = "<<song_id;
	LOG_DEBUG2("%s",os.str().c_str());
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		LOG_ERROR("sqlexec error");
		return r;
	}
	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			/*hq_url = "http://www.baidu.com/1.mp3";
			song_url = "http://www.baidu.com/1.mp3";
			clt_num = rows[0];
			cmt_num = rows[1];
			hot_num = rows[2];
			*/
			if (rows[0] !=NULL){
				hq_url.assign(rows[1]);
				song_url = hq_url;
				clt_num.assign(rows[3]);
				cmt_num.assign(rows[4]);
				hot_num.assign(rows[5]);
			}
		}
		return true;
	}

	return true;

}
bool DBComm::GetMusicFriendNum(const std::string &uid,
							   std::string& snum){
	std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
#if defined (_DB_SINGLE_)
		mig_lbs::RLockGd lk(db_single_lock_);
		base_storage::DBStorageEngine* engine = db_conn_single_;
#endif
	if (engine==NULL){
		LOG_ERROR("engine error");
		return true;
	}
	base_storage::db_row_t* db_rows;
	int num;
	MYSQL_ROW rows = NULL;
	os<<"select count(*) from migfm_user_music_friend where taruserid=\'"
		<<uid.c_str()<<"\';";

	LOG_DEBUG2("%s",os.str().c_str());

	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			if (rows[0] !=NULL)
				snum.assign(rows[0]);
		}
		return true;
	}

	return false;
}

}
