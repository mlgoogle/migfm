/*
 * db_comm.cc
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#include "db_comm.h"
#include "db/base_db_mysql_auto.h"
#include <sstream>
#include <mysql.h>

namespace socsvc_logic{

void DBComm::Init(std::list<base::ConnAddr>& addrlist){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Init(addrlist);
#endif
}

void DBComm::Dest(){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Dest();
#endif
}

bool DBComm::GetTypeBarrage(const int64 platform,const int64 groupid,
		const int64 count,std::list<socsvc_logic::BarrageInfos>& list){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}

    //call proc_V2GetTypeBarrage(10000,20001,10)
	os<<"call proc_V2GetTypeBarrage("<<platform<<","<<groupid
			<<","<<count<<")";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}


	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			socsvc_logic::BarrageInfos barrage;
			if(rows[0]!=NULL)
				barrage.set_msg_id(atoll(rows[0]));
			if(rows[1]!=NULL)
				barrage.set_fid(atoll(rows[1]));
			if(rows[2]!=NULL)
				barrage.set_nickname(rows[2]);
			if(rows[3]!=NULL)
				barrage.set_message(rows[3]);
			list.push_back(barrage);
		}
		return true;
	}
	return false;
}

}
