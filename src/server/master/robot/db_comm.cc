#include "db_comm.h"
#include "thread_handler.h"
#include "basic/basic_info.h"
#include "basic/base64.h"
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
	robot_logic::WLockGd lk(db_pool_lock_);
	db_conn_pool_.push_back(engine);
}

base_storage::DBStorageEngine* DBComm::DBConnectionPop(){
	if(db_conn_pool_.size()<=0)
		return NULL;
	robot_logic::WLockGd lk(db_pool_lock_);
    base_storage::DBStorageEngine* engine = db_conn_pool_.front();
    db_conn_pool_.pop_front();
    return engine;
}

#endif


void DBComm::Dest(){
#if defined (_DB_POOL_)
	robot_logic::WLockGd lk(db_pool_lock_);
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


bool DBComm::GetRobotsInfo(const int64 count, const int64 from,
		std::list<robot_base::RobotInfo>& user_list){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		MYSQL_ROW rows;

		if (engine==NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}
		//call migfm.proc_GetRobotsInfo(10,0)
		os<<"call migfm.proc_GetRobotsInfo("
			<<count<<","<<from<<");";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		bool r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}

		int num = engine->RecordCount();
		if(num>0){
			while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
				int64 uid = atoll(rows[0]);
				LOG_DEBUG2("uid %d",uid);
				std::string nickname = rows[1];
				std::string sex = rows[2];
				std::string head_url = rows[3];
				robot_base::RobotInfo robotinfo(uid,nickname,sex,head_url);
				user_list.push_back(robotinfo);
			}
			return true;
		}
		return false;
}


bool DBComm::GetMailUserInfo(const int64 count,const int64 from,
					std::list<robot_base::MailUserInfo>& user_list){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		MYSQL_ROW rows;

		if (engine==NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}
		//call migfm.proc_GetUserInfoMail(10,0)
		os<<"call migfm.proc_GetUserInfoMail("
			<<from<<","<<count<<");";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		bool r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}

		int num = engine->RecordCount();
		if(num>0){
			while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
				int64 uid = atoll(rows[0]);
				std::string username = rows[1];
				std::string nickname = rows[2];
				robot_base::MailUserInfo mailinfo(uid,username,nickname);
				user_list.push_back(mailinfo);
			}
			return true;
		}
		return false;
}

bool DBComm::AddMusicInfo(const int64 id,const std::string& title,const std::string& album,const std::string& artist,
		const std::string& pub_time,const std::string& album_pic,const std::string& mp3_url,std::string& songid){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		MYSQL_ROW rows;

		if (engine==NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}

	    //call migfm.proc_AddNewMusic('5Zac5qyi5L2g','WHBvc2Vk','Ry5FLk0u6YKT57Sr5qOL','0','http://img2.kuwo.cn/star/albumcover/120/36/89/2454180231.jpg','http://ra01.sycdn.kuwo.cn/resource/n1/93/12/2889920061.mp3')
		os<<"call proc_AddNewMusic("<<id<<",\'"
			<<title<<"\',\'"<<album<<"\',\'"
			<<artist<<"\',\'"<<pub_time<<"\',\'"
			<<album_pic<<"\',\'"<<mp3_url<<"\');";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		bool r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}


		int num = engine->RecordCount();
		if(num>0){
			while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
				songid = rows[0];
			}
			return true;
		}
		return false;
}

bool DBComm::GetSpreadMail(std::string& title,std::string& content){
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}

    //call migfm.proc_GetMailMessage();
	os<<"call proc_GetMailMessage();";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}


	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			title = rows[0];
			content = rows[1];
		}
		return true;
	}
	return false;
}

bool DBComm::GetNewMusicInfo(std::list<robot_base::NewMusicInfo>& music_list){
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}

    //call migfm.proc_GetNewMusicInfo(;
	os<<"call proc_GetNewMusicInfo();";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}


	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			int64 id = atoll(rows[0]);
			std::string name = rows[1];
			std::string singer = rows[2];
			robot_base::NewMusicInfo musicinfo(id,name,singer);
			music_list.push_front(musicinfo);
		}
		return true;
	}
	return false;
}

bool DBComm::UpdateHeadUrl(const int64 id,const std::string& head_url){
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
		std::stringstream os;
		MYSQL_ROW rows;

		if (engine==NULL){
			LOG_ERROR("GetConnection Error");
			return false;
		}

	    //call migfm.proc_UpdateHeadUrl(10001,'http://pic.headurl.com/1.jpg')
		os<<"call proc_UpdateHeadUrl("<<id<<",\'"<<head_url<<"\');";
		std::string sql = os.str();
		LOG_DEBUG2("[%s]", sql.c_str());
		bool r = engine->SQLExec(sql.c_str());

		if (!r) {
			LOG_ERROR2("exec sql error");
			return false;
		}
		return true;
}

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
		bool r = false;
		base_storage::DBStorageEngine* engine = robot_logic::ThreadKey::GetStorageDBConn();
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
		robot_logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

}
