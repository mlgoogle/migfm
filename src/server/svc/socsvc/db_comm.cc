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


double DBComm::GetDistance(const int64 uid,const int64 tid){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	double u_latitude = 0;
	double u_longitude = 0;
	double t_latitude = 0;
	double t_longitude = 0;
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return 0;
	}

    //call proc_V2GetUserBatchLbsInfo('10151,10251')
	os<<"call proc_V2GetUserBatchLbsInfo(\'"<<uid<<","<<tid<<"\')";
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
			if(atoll(rows[0])==uid){
				u_latitude = atof(rows[1]);
				u_longitude = atof(rows[2]);
			}else{
				t_latitude = atof(rows[1]);
				t_longitude = atof(rows[2]);
			}
		}
		return base::BasicUtil::CalcGEODistance(u_latitude,u_longitude,t_latitude,t_longitude);
	}
	return 0;
}

bool DBComm::GetMyFriend(const int64 uid,const int64 from,const int64 count,
		std::list<base_logic::UserAndMusic>& list){
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
	os<<"call proc_V2GetHistoryFriend("<<uid<<","<<from<<","<<count<<")";
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
			list.push_back(info);
		}
	}
	return 0;
}

}
