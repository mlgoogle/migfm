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
		base_storage::DBStorageEngine* engine = usr_logic::ThreadKey::GetStorageDBConn();
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
		usr_logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
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

	//推广用 用于心情1
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

bool DBComm::GetMoodParentWord(std::list<base::WordAttrInfo>& word_list){
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
	os<<"select id,name from migfm_mood;";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			//song_url = rows[0];
			std::string id;
			std::string name;
			id = rows[0];
			name = rows[1];
			base::WordAttrInfo word_attr(id,name);
			word_list.push_back(word_attr);
		}
	}
}

bool DBComm::GetMusicUrl(const std::string& song_id,std::string& song_url){
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
	return true;
}

bool DBComm::GetDescriptionWord(std::list<base::WordAttrInfo> &word_list, int flag){
	std::stringstream os;
	bool r = false;
	base_storage::DBStorageEngine* engine = GetConnection();
	base_storage::db_row_t* db_rows;
	int num;
	MYSQL_ROW rows = NULL;
	if (flag){
		os<<"select name,typeid from migfm_mood_word order by rand() limit 9;";
	}else{
		os<<"select name,typeid from migfm_scene_word order by rand() limit 9;";
	}

	if (engine==NULL){
		LOG_ERROR("engine error");
		return true;
	}

	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			std::string name = rows[0];
			std::string type_id = rows[1];
			base::WordAttrInfo word(type_id,name);
			word_list.push_back(word);
		}
	}
	return true;
}

bool DBComm::GetChannelInfo(std::vector<base::ChannelInfo>& channel,int& num){
	std::stringstream os;
	bool r = false;
	base_storage::DBStorageEngine* engine = GetConnection();
	base_storage::db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	os<<"select id,channel_id,channel_name,channel_pic from migfm_channel";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error ");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){

			std::string id = rows[0];
			std::string channel_id = rows[1];
			std::string channel_name = rows[2];
			std::string channel_pic = rows[3];
			MIG_INFO(USER_LEVEL,"id[%s] channel[%s] channel_name[%s]",
				id.c_str(),channel_id.c_str(),channel_name.c_str());
			base::ChannelInfo ci(id,channel_id,channel_name,channel_pic);
			channel.push_back(ci);
		}
	}
	return true;
}

}