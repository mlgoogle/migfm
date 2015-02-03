/*
 * db_comm.cc
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#include "db_comm.h"
#include "db/base_db_mysql_auto.h"
#include "basic/basic_util.h"
#include <sstream>
#include <mysql.h>

namespace musicsvc_logic{

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

bool DBComm::GetAllDimension(std::list<base_logic::Dimension>& list){
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

    //call proc_V2GetAllDimension()
	os<<"call proc_V2GetAllDimension()";
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
			base_logic::Dimension  dimension;
			if(rows[0]!=NULL)
				dimension.set_id(atol(rows[0]));
			if(rows[1]!=NULL)
				dimension.set_class_name(rows[1]);
			if(rows[2]!=NULL)
				dimension.set_name(rows[2]);
			if(rows[3]!=NULL)
				dimension.set_description(rows[3]);
			list.push_back(dimension);
		}
		return true;
	}
	return false;
}

}
