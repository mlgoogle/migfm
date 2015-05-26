/*
 * data_comm.cc
 *
 *  Created on: 2015年5月25日
 *      Author: kerry
 *  Time: 下午11:06:49
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */

#include "data_db_comm.h"
#include "db/base_db_mysql_auto.h"
#include "basic/basic_util.h"
#include <sstream>
#include <mysql.h>
namespace base_logic{


DataDBComm::DataDBComm(){
	mysql_engine_.reset(base_logic::DataStorageBaseEngine::Create(MYSQL_TYPE));
}

DataDBComm::~DataDBComm(){

}

bool DataDBComm::ReadUserInfo(const int64 uid,base_logic::UserInfo& info){
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	std::string sql = "call proc_V2GetUserInfo("+base::BasicUtil::BasicUtil::StringUtil::Int64ToString(uid)+")";
	mysql_engine_->ReadData(sql,(base_logic::Value*)(dict.get()),CallBackReadUserInfo);
	return info.ValueSerialization(dict.get());
}

void DataDBComm::CallBackReadUserInfo(void* param,base_logic::Value* value){
	base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)(value);
	base_storage::DBStorageEngine* engine  = (base_storage::DBStorageEngine*)(param);
	MYSQL_ROW rows;
	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			if(rows[0]!=NULL)
				dict->SetBigInteger(L"uid",atoll(rows[0]));
			if(rows[1]!=NULL)
				dict->SetInteger(L"sex",atol(rows[1]));
			if(rows[2]!=NULL)
				dict->SetInteger(L"type",atol(rows[2]));
			if(rows[3]!=NULL)
				dict->SetString(L"nickname",std::string(rows[3]));
			if(rows[4]!=NULL)
				dict->SetInteger(L"source",atol(rows[4]));
			if(rows[5]!=NULL)
				dict->SetInteger(L"machine",atol(rows[5]));
			if(rows[6]!=NULL)
				dict->SetString(L"city",std::string(rows[6]));
			if(rows[7]!=NULL)
				dict->SetString(L"birthday",std::string(rows[7]));
			if(rows[8]!=NULL)
				dict->SetBigInteger(L"logintime",atoll(rows[8]));
			if(rows[9]!=NULL)
				dict->SetString(L"head",std::string(rows[9]));
		}
		return;
	}
}


/*
DataDBComm::DataDBComm(){

}

DataDBComm::~DataDBComm(){

}

void DataDBComm::Init(std::list<base::ConnAddr>& addrlist){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Init(addrlist);
#endif
}

void DataDBComm::Dest(){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Dest();
#endif
}

bool DataDBComm::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
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

    //call migfm.proc_V2GetUserInfo(10108)
	os<<"call proc_V2GetUserInfo("<<uid<<")";
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
				info.set_uid(atoll(rows[0]));
			if(rows[1]!=NULL)
				info.set_sex(atol(rows[1]));
			if(rows[2]!=NULL)
				info.set_type(atol(rows[2]));
			if(rows[3]!=NULL)
				info.set_nickname(rows[3]);
			if(rows[4]!=NULL)
				info.set_source(atol(rows[4]));
			if(rows[5]!=NULL)
				info.set_machine(atol(rows[5]));
			if(rows[6]!=NULL)
				info.set_city(rows[6]);
			if(rows[7]!=NULL)
				info.set_birthday(rows[7]);
			if(rows[8]!=NULL)
				info.set_logintime(atoll(rows[8]));
			if(rows[9]!=NULL)
				info.set_head(rows[9]);
		}
		return true;
	}
	return false;
}*/


}



