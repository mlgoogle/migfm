#include "db_comm.h"
#include "logic_comm.h"
#include "thread_handler.h"
#include <mysql.h>
#include <sstream>

#define USERINFOS "migfm_user_infos"

namespace storage {

std::list<base::ConnAddr> DBComm::addrlist_;

void DBComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void DBComm::Dest(){

}

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
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
		engine->Connections(addrlist_);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
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

	base_storage::DBStorageEngine* engine = GetConnection();
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
		if (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
			nickname = rows[0];
			gender = rows[1];
			pic = rows[2];
		}
		return true;
	}
	return false;
}

bool DBComm::GetUserLbsPos(const int64 src_uid,double& latitude, 
						   double& longitude){
							   
	base_storage::DBStorageEngine* engine = GetConnection();
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
		if (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
			latitude = atof(rows[0]);
			longitude = atof(rows[1]);
		}
		return true;
	}
	return false;
}

bool DBComm::UpDateUserLbsPos(Json::Value& users,const int64 src_uid){

	base_storage::DBStorageEngine* engine = GetConnection();
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
	}

	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
    return true;
}

bool DBComm::GetMusicUrl(const std::string &song_id, std::string &hq_url, std::string &song_url){
	std::stringstream os;
	bool r = false;
	base_storage::DBStorageEngine* engine = GetConnection();
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


bool DBComm::GetMusicFriendNum(const std::string &uid,
							   std::string& snum){
	std::stringstream os;
	bool r = false;
	base_storage::DBStorageEngine* engine = GetConnection();
	if (engine==NULL){
		LOG_ERROR("engine error");
		return true;
	}
	base_storage::db_row_t* db_rows;
	int num;
	MYSQL_ROW rows = NULL;
	os<<"select count(*) from migfm_user_music_friend where userid=\'"
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

	return true;
}

}
