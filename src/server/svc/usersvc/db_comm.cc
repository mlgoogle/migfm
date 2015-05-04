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

namespace usersvc_logic{

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

bool DBComm::OnThirdLogin(usersvc_logic::UserInfo& userinfo,
		base_logic::LBSInfos* lbsinfo){
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

    //call proc_V2ThirdLogin('123123',1,1,'北京','老K','s123123123',1,'1986','http://123123')
	os<<"call proc_V2ThirdLogin(\'"<<userinfo.imei()<<"\',"<<userinfo.machine()
			<<","<<userinfo.type()<<",\'"<<userinfo.city()<<"\',\'"<<userinfo.nickname()
			<<"\',\'"<<userinfo.session()<<"\',"<<userinfo.sex()<<",\'"<<userinfo.birthday()
			<<"\',\'"<<userinfo.head()<<"\',"<<userinfo.plt()<<");";
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
			if(rows[0]!=NULL)
				userinfo.set_uid(atoll(rows[0]));
			if(rows[1]!=NULL)
				userinfo.set_sex(atol(rows[1]));
			if(rows[2]!=NULL)
				userinfo.set_nickname(rows[2]);
			if(rows[3]!=NULL)
				userinfo.set_city(rows[3]);
			if(rows[4]!=NULL)
				userinfo.set_type(atol(rows[4]));
			if(rows[5]!=NULL)
				userinfo.set_machine(atol(rows[5]));
			if(rows[6]!=NULL)
				userinfo.set_machine(atol(rows[6]));
			if(rows[7]!=NULL)
				userinfo.set_head(rows[7]);
			return true;
		}
		return true;
	}
	return false;
}

bool DBComm::OnQuickRegister(usersvc_logic::UserInfo& userinfo,
		base_logic::LBSInfos* lbsinfo){
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

    //call proc_QuickRegister
	os<<"call buddha.proc_QuickRegister(\'"<<userinfo.imei()<<"\',"<<userinfo.machine()
			<<",\'"<<lbsinfo->city()<<"\')";

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
			if(rows[0]!=NULL)
				userinfo.set_uid(atoll(rows[0]));
			if(rows[1]!=NULL)
				userinfo.set_sex(atol(rows[1]));
			if(rows[2]!=NULL)
				userinfo.set_nickname(rows[2]);
			if(rows[3]!=NULL)
				userinfo.set_city(rows[3]);
			if(rows[4]!=NULL)
				userinfo.set_type(atol(rows[4]));
			if(rows[5]!=NULL)
				userinfo.set_machine(atol(rows[5]));
			if(rows[6]!=NULL)
				userinfo.set_head(rows[6]);
			return true;
		}
		return true;
	}
	return false;
}

bool DBComm::OnLoginRecord(const int64 uid,const int32 plt,base_logic::LBSInfos* lbsinfo){
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

    //call migfm.proc_V2LoginRecord(10300,'杭州','120.143000','30.286500')
	os<<"call proc_V2LoginRecord("<<uid<<",\'"<<lbsinfo->city()<<"\',\'"<<lbsinfo->latitude()<<"\',\'"
			<<lbsinfo->longitude()<<"\',"<<plt<<")";

	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}
	return true;

}

bool DBComm::OnUpdateUserInfo(usersvc_logic::UserInfo& userinfo){
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


	 os<<"call proc_V2UpdateUserInfo("<<userinfo.uid()<<","<<userinfo.sex()<<",\'"
			 <<userinfo.nickname()<<"\',\'"<<userinfo.birthday()<<"\');";
	 LOG_DEBUG2("[%s]",os.str().c_str());
	 r = engine->SQLExec(os.str().c_str());

	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return false;
	}
	return true;
}

}
