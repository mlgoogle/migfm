#include "db_comm.h"
#include "logic_comm.h"
#include "thread_handler.h"
#include "basic/basic_info.h"
#include "basic/base64.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace mig_sociality {

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
		base_storage::DBStorageEngine* engine = mig_sociality::ThreadKey::GetStorageDBConn();
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
		engine->Connections(addrlist_);
		mig_sociality::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool DBComm::GetMusicUser(const std::string& uid, 
						  const std::string& fromid, 
						  const std::string& count, 
						  std::vector<std::string>& vec_users,
						  std::list<base::UserInfo>& userlist){
	  std::stringstream os;
	  std::string sql;
	  MYSQL_ROW rows;
	  base_storage::DBStorageEngine* engine = GetConnection();
	  if (engine==NULL){
		  LOG_ERROR("engine error");
		  return false;
	  }

	  // proc_GetHistoryFriends
	  os<< "call proc_GetHistoryFriends("
		  << uid.c_str() << ","
		  << fromid.c_str() << ","
		  << count.c_str() << ")";

	  sql = os.str();
	  engine->SQLExec(sql.c_str());
	  LOG_DEBUG2("%s",sql.c_str());
	  int32 num = engine->RecordCount();
	  if(num>0){
		  while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			  base::UserInfo userinfo;
			  userinfo.set_nickname(rows[0]);
			  userinfo.set_sex(rows[1]);
			  userinfo.set_uid(rows[2]);
			  userinfo.set_head(rows[3]);
			  userinfo.set_source(rows[4]);
			  vec_users.push_back(userinfo.uid());
			  userlist.push_back(userinfo);
			  //userlist.push_back(rows[0]);
		  }
		  return true;
	  }
	  return false;
}

bool DBComm::GetUserInfos(const std::string& uid, std::string& nickname,
		std::string& gender, std::string& head) {
	nickname.clear();
	gender.clear();
	head.clear();

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
			head = rows[2];
		}
		return true;
	}
	return false;
}

bool DBComm::GetMusicUrl(const std::string& song_id,std::string& hq_url,
						 std::string& song_url){
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

			 if (rows[1] ==NULL)
				 song_url = DEFAULT_URL;
			 else
				 song_url = rows[1];
		 }
		 return true;
	 }else{
		 hq_url = song_url = DEFAULT_URL;
		 return false;
	 }
	 return true;
}

bool DBComm::GetWXMusicUrl(const std::string& song_id,std::string& song_url,
						   std::string& dec,std::string& dec_id,std::string& dec_word){
	std::stringstream os;
	std::stringstream os1;
	bool r = false;
	base_storage::DBStorageEngine* engine = GetConnection();
	if (engine==NULL){
	   LOG_ERROR("engine error");
	   return true;
	}
	base_storage::db_row_t* db_rows;
	int num;
	MYSQL_ROW rows = NULL;
	os<<"select song_url from migfm_music_url where song_id =\'"
	   <<song_id.c_str()<<"\';";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
	   MIG_ERROR(USER_LEVEL,"sqlexec error");
	   return r;
	}

	num = engine->RecordCount();
	if(num>0){
	   while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
		   song_url = rows[0];
	   }
	}

	//mood scens
	int current = time(NULL)%2;


	dec = "mm";
	os.str("");
	os<<" select name,typeid from migfm_mood_word where typeid=1 ORDER BY RAND() limit 1;";
// 	if (current==0){//mood
// 		dec="mm";
// 		os.str("");
// 		os<<"select a.name,typeid from migfm_mood_word as a join migfm_mood as b where a.typeid = b.id  ORDER BY RAND() limit 1;";
// 	}else{//scens
// 		dec="ms";
// 		os.str("");
// 		os<<"select a.name,typeid from migfm_scene_word as a join migfm_scene as b where a.typeid = b.id  ORDER BY RAND() limit 1;";
// 	}

	MIG_DEBUG(USER_LEVEL,"%s",os.str().c_str());
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = engine->RecordCount();
 	if(num>0){
 		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
 			dec_id = rows[1];
			Base64Decode(rows[0],&dec_word);
 		}
 	}

	return true;
}

bool DBComm::AddMusciFriend(const std::string& uid, 
							const std::string &touid){
	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}

	// proc_AddFriend
	os	<< "call proc_RecordMusicFriend("
		<< uid.c_str() << ","
		<< touid.c_str() << ")";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	return true;
}

bool DBComm::AddFriend(const std::string& uid, const std::string& touid) {
	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}

	// proc_AddFriend
	os	<< "call proc_AddFriend("
		<< uid.c_str() << ","
		<< touid.c_str() << ")";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	return true;
}

bool DBComm::GetFriendList(const std::string& uid, FriendInfoList& friends) {
	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}

	// proc_GetUserFriends
	os	<< "call proc_GetUserFriends("
		<< uid.c_str() << ")";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}

	int num = engine->RecordCount();
 	if(num > 0){
 		friends.reserve(num);
 		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
 			FriendInfo finfo;
 			finfo.uid = rows[0];
 			finfo.name = rows[1];
 			finfo.type = atoi(rows[2]);
 			friends.push_back(finfo);
 		}
 	}

	return true;
}

bool DBComm::GetUserInfos(int64 uid,
						  std::string& nickname,std::string& gender,
						  std::string& type,std::string& birthday,
						  std::string& location,std::string& source,
						  std::string& head){
	nickname.clear();
	gender.clear();
	type.clear();
	birthday.clear();
	location.clear();
	source.clear();
	head.clear();

	  base_storage::DBStorageEngine* engine = GetConnection();
	  std::stringstream os;
	  bool r = false;
	  MYSQL_ROW rows;
	  if (engine==NULL){
		  LOG_ERROR("GetConnection Error");
		  return false;
	  }
	  os<<"select sex,type,ctry,birthday,nickname,source,head from "
		  <<USERINFOS<<" where usrid=" << uid << ";";
	  const char* sql = os.str().c_str();
	  LOG_DEBUG2("[%s]",os.str().c_str());
	  r = engine->SQLExec(os.str().c_str());

	  if (!r){
		  LOG_ERROR2("exec sql error");
		  return false;
	  }
	  int32 num = engine->RecordCount();
	  if(num>0){
		  while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			  gender = rows[0];
			  type = rows[1];
			  location = rows[2];
			  birthday = rows[3];
			  nickname = rows[4];
			  source = rows[5];
			  head = rows[6];
		  }
		  return true;
	  }
	  return false;

}

} // mig_sociality
