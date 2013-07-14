#include "db_comm.h"
#include "logic_comm.h"
#include "thread_handler.h"
#include "basic/basic_info.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace storage{

std::list<base::ConnAddr> DBComm::addrlist_;

void DBComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void DBComm::Dest(){

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
		engine->Connections(addrlist_);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		usr_logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
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
		MIG_ERROR(USER_LEVEL,"sqlexec error");
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