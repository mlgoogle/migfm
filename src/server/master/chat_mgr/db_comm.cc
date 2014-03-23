#include "db_comm.h"
#include "thread_handler.h"
#include "basic/basic_info.h"
#include "basic/base64.h"
#include "storage/storage.h"
#include <mysql.h>
#include <sstream>

namespace chat_storage{

std::list<base::ConnAddr> DBComm::addrlist_;

DBComm::DBComm(){

}

DBComm::~DBComm(){

}

void DBComm::Init(std::list<base::ConnAddr>& addrlist,
				  const int32 db_conn_num/* = 10*/){
	addrlist_ = addrlist;
}

void DBComm::Dest(){

}

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
		bool r = false;
		base_storage::DBStorageEngine* engine = chat_logic::ThreadKey::GetStorageDBConn();
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
		chat_logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool DBComm::GetLeaveMessage(const int64 platform_id,const int64 uid,const int64 oppid,
							 const int32 from,const int32 count,
                             std::list<struct GetLeaveMessage*>& list){
	bool r = false;
	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}

    //call migfm.proc_GetChatMessage(1000,10149,10108,10,0);
	os<<"call proc_GetChatMessage(\'"<<platform_id<<"\',\'"<<uid<<"\',\'"
			<<oppid<<"\',\'"<<count<<"\',\'"<<from<<"\');";
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
			struct GetLeaveMessage* msg = new struct GetLeaveMessage;
			msg->platform_id = rows[0];
			msg->msg_id = rows[1];
			msg->uid = rows[2];
			msg->oppid = rows[3];
			msg->message = rows[4];
			msg->lasttime = rows[5];
			list.push_back(msg);
		}
		return true;
	}
	return false;
}

}
