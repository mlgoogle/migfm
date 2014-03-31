#include "db_comm.h"
#include "base/thread_handler.h"
#include "base/logic_comm.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace chat_storage{

std::list<base::ConnAddr> DBComm::addrlist_;

void DBComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void DBComm::Dest(){

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

bool DBComm::RecordMessage(const int64 platform_id,const int64 fid,const int64 tid,
					const int64 msg_id,const std::string& message,
					const std::string& current_time){

	base_storage::DBStorageEngine* engine = GetConnection();
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

bool DBComm::GetUserInfo(const int64 platform_id,int64 user_id, 
						 chat_base::UserInfo& userinfo){
    
     // call proc_ChatGetUserInfo(platform_id,user_id)
	base_storage::DBStorageEngine* engine = GetConnection();
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

}
