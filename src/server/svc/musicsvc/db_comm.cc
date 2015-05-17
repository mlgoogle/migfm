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

bool DBComm::RecordMusicHistory(const int64 uid,const int64 songid){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return 0;
	}

    //call proc_RecordMusicHistory(10149,1011111)
	os<<"call proc_RecordMusicHistory("<<uid<<","<<songid<<")";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}

	return true;
}

bool DBComm::GetMyFriend(const int64 uid,const int64 from,const int64 count,
		std::map<int64,base_logic::UserAndMusic>& map){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return 0;
	}

    //call migfm.proc_V2GetHistoryFriend(10149,0,10)
	os<<"call proc_V2GetHistoryFriend("<<10149<<","<<from<<","<<count<<")";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return 0;
	}


	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			base_logic::UserAndMusic info;
			if(rows[0]!=NULL)
				info.userinfo_.set_uid(atoll(rows[0]));
			if(rows[1]!=NULL)
				info.userinfo_.set_nickname(rows[1]);
			if(rows[2]!=NULL)
				info.userinfo_.set_sex(atol(rows[2]));
			if(rows[3]!=NULL)
				info.userinfo_.set_source(atol(rows[3]));
			if(rows[4]!=NULL)
				info.lbsinfo_.set_latitude(atof(rows[4]));
			if(rows[5]!=NULL)
				info.lbsinfo_.set_longitude(atof(rows[5]));
			if(rows[6]!=NULL)
				info.userinfo_.set_birthday(rows[6]);
			if(rows[7]!=NULL)
				info.userinfo_.set_head(rows[7]);
			if(rows[8]!=NULL)
				info.userinfo_.set_logintime(atoll(rows[8]));
			map[info.userinfo_.uid()] = info;
		}
	}
	return 0;
}
}
