#include "db_comm.h"
#include "thread_handler.h"
#include "basic/basic_info.h"
#include "basic/base64.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace storage{

#if defined (_DB_POOL_)
threadrw_t* DBComm::db_pool_lock_;
std::list<base_storage::DBStorageEngine*>  DBComm::db_conn_pool_;
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
				  const int32 db_conn_num/* = 5*/){
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
}

#if defined (_DB_POOL_)

void DBComm::DBConnectionPush(base_storage::DBStorageEngine* engine){
	music_logic::WLockGd lk(db_pool_lock_);
	db_conn_pool_.push_back(engine);
}

base_storage::DBStorageEngine* DBComm::DBConnectionPop(){
	if(db_conn_pool_.size()<=0)
		return NULL;
	music_logic::WLockGd lk(db_pool_lock_);
    base_storage::DBStorageEngine* engine = db_conn_pool_.front();
    db_conn_pool_.pop_front();
    return engine;
}

#endif


void DBComm::Dest(){
#if defined (_DB_POOL_)
	music_logic::WLockGd lk(db_pool_lock_);
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
		base_storage::DBStorageEngine* engine = music_logic::ThreadKey::GetStorageDBConn();
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
		music_logic::ThreadKey::SetStorageDBConn(engine);
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
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
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
 		return true;
 	}

	return true;
}

bool DBComm::GetUserHistoryMusic(const std::string& uid,const std::string& fromid, 
								 const std::string& count,
								 std::list<std::string>& songlist){
	 bool r = false;
	 std::stringstream os;
	 std::string sql;
	 MYSQL_ROW rows;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	 if (engine==NULL){
		 LOG_ERROR("engine error");
		 return false;
	 }

	 os<<"select songid from migfm_user_music_history  where userid = \'"
		 <<uid.c_str()<<"\' order by lasttime desc limit "<<fromid.c_str()
		 <<","<<count.c_str()<<";";
	 sql = os.str();
	 engine->SQLExec(sql.c_str());
	 LOG_DEBUG2("%s",sql.c_str());
	 int32 num = engine->RecordCount();
	 if(num>0){
		 while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			 songlist.push_back(rows[0]);
		 }
		 return true;
	 }
	 return false;
}


bool DBComm::UpdateLogin(const int64 uid){
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	if (engine==NULL){
		LOG_ERROR("engine error");
		return false;
	}
	/*
	call migfm.proc_UpdateLoginTime(10108)
	*/
	os<<"call proc_UpdateLoginTime("<<uid<<");";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	return true;
}
bool DBComm::RecordMusicHistory(const std::string& uid,const std::string& songid){
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	if (engine==NULL){
		LOG_ERROR("engine error");
		return false;
	}
	/*
	call proc_RecordMusicHistory('100000','1232323');
	*/
	os<<"call proc_RecordMusicHistory(\'"<<uid.c_str()<<"\',\'"<<songid.c_str()
		<<"\');";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	return true;
}

bool DBComm::GetSongidFromDoubanId(const std::string& douban_songid,std::string& songid){
    std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	if (engine==NULL){
		LOG_ERROR("engine error");
		return true;
	}
	base_storage::db_row_t* db_rows;
	int num;
	MYSQL_ROW rows = NULL;
	os<<"select id from migfm_music_douban where sidx = \""
		<<douban_songid.c_str()<<"\";";
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			songid = rows[0];
		}
		return true;
	}
	return false;
}

bool DBComm::SetMusicHostCltCmt(const std::string& songid,
								const int32 flag, 
								const int32 value){
	
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	if (engine==NULL){
		LOG_ERROR("engine error");
		return false;
	}
	/*
	call proc_SetMuiscAbout`('100000',1,1);
	*/
	os<<"call proc_SetMuiscAbout(\'"<<songid.c_str()<<"\',"
		<<flag<<","<<value<<");";

	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	return true;
}


bool DBComm::GetMoodParentWord(std::list<base::WordAttrInfo>& word_list){
	std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
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
		return true;
	}
	return true;
}

bool DBComm::GetMusicUrl(const std::string& song_id,std::string& hq_url,
						 std::string& song_url){
	std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
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

			if (rows[1] ==NULL)
				song_url = DEFAULT_URL;
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

bool DBComm::GetDescriptionWord(std::list<base::WordAttrInfo> &word_list, int flag){
	std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
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
		return true;
	}
	return false;
}

bool DBComm::GetChannelInfo(std::vector<base::ChannelInfo>& channel,int& num){
	std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	base_storage::db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	os<<"select id,channel_id,channel_name,channel_pic,description from migfm_channel";
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
			std::string channel_dec = rows[4];
			//MIG_INFO(USER_LEVEL,"id[%s] channel[%s] channel_name[%s]",
				//id.c_str(),channel_id.c_str(),channel_name.c_str());
			base::ChannelInfo ci(id,channel_id,channel_name,channel_pic);
			channel.push_back(ci);
		}
		return true;
	}
	return true;
}


bool DBComm::GetMusicOtherInfos(std::map<std::string,base::MusicInfo>&song_music_infos){

	std::stringstream os;
	bool r = false;
#if defined (_DB_POOL_)
		AutoDBCommEngine auto_engine;
		base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	base_storage::db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	int num = song_music_infos.size();
	if (num==0)
		return false;
	os<<"call proc_GetMusicInfo('";
	for(std::map<std::string,base::MusicInfo>::iterator it = song_music_infos.begin();
		it!=song_music_infos.end();++it){
		
		os<<it->second.id();
		num--;
		if (num!=0){
			os<<",";
		}
	}

	os<<"');";
	LOG_DEBUG2("%s",os.str().c_str());
	r = engine->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error ");
		return r;
	}

	num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			std::string songdid = rows[1];
			std::string hifi_url = rows[2];
			//std::string url = rows[3];
			std::string clt_num = rows[4];
			std::string cmt_num = rows[5];
			std::string hot_num = rows[6];
			std::map<std::string,base::MusicInfo>::iterator itr 
				=song_music_infos.find(songdid);
			if (itr!=song_music_infos.end()){
				itr->second.set_hq_url(hifi_url);
				itr->second.set_url(hifi_url);
				itr->second.set_music_clt(clt_num);
				itr->second.set_music_cmt(cmt_num);
				itr->second.set_music_hot(hot_num);
			}
		}
		return true;
	}

	return false;
}

bool DBComm::GetChannelInfos(std::list<int>& list){
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
			int channel_id = atol(rows[0]);
			list.push_back(channel_id);
		}
	}
	return true;
}

bool DBComm::GetMoodInfos(std::list<int> &list){
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

bool DBComm::GetSceneInfos(std::list<int> &list){
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


}
