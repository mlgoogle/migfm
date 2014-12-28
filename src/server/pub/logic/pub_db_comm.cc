/*
 * pub_db_comm.cc
 *
 *  Created on: 2014年12月11日
 *      Author: kerry
 */


#include "pub_db_comm.h"
#include "db/base_db_mysql_auto.h"
#include <sstream>
#include <mysql.h>

namespace basic_logic{

void PubDBComm::Init(std::list<base::ConnAddr>& addrlist){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Init(addrlist);
#endif
}

void PubDBComm::Dest(){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Dest();
#endif
}

void PubDBComm::GetDimensions(const std::string& type,
		base_logic::Dimensions& dimensions){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return ;
	}
	//call migfm.proc_V2GetDimensions('chl')
	os<<"call migfm.proc_V2GetDimensions(\'"<<type<<"\')";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return ;
	}

	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			int64 id = 0;
			std::string name;
			std::string desc;
			if(rows[0]!=NULL)
				id = atoll(rows[0]);
			if(rows[1]!=NULL)
				name = rows[1];
			base_logic::Dimension dimension(id,name,desc);
			dimensions.set_dimension(id,dimension);
		}
	}
}


}
