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

namespace chatsvc_logic{

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

bool DBComm::OnGetLeaveMessage(const int64 platform,const int64 fid,const int64 tid,const int64 from,
		const int64 count,
			const int64 msgid,std::list<chatsvc_logic::AloneMessageInfos>& list){
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

    //call migfm.proc_V2GetAloneMessage(10000,10149,10150,10,0,0)
	os<<"call migfm.proc_V2GetAloneMessage("<<platform<<","
		<<fid<<","<<tid<<","<<count<<","<<from<<","<<msgid<<")";
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
			chatsvc_logic::AloneMessageInfos alone_msg;
			if(rows[0]!=NULL)
				alone_msg.set_platform(atoll(rows[0]));
			if(rows[1]!=NULL)
				alone_msg.set_msgid(atoll(rows[1]));
			if(rows[2]!=NULL)
				alone_msg.set_fid(atoll(rows[2]));
			if(rows[3]!=NULL)
				alone_msg.set_tid(atoll(rows[3]));
			if(rows[4]!=NULL)
				alone_msg.set_nickname(rows[4]);
			if(rows[5]!=NULL)
				alone_msg.set_head(rows[5]);
			if(rows[6]!=NULL)
				alone_msg.set_message(rows[6]);
			if(rows[7]!=NULL)
				alone_msg.set_lasttime(rows[7]);
			list.push_back(alone_msg);
		}
		return true;
	}
	return false;
}

bool DBComm::OnGetGroupMessage(const int64 platform,const int64 groupid,const int32 from,const int32 count,
			const int64 msgid,std::list<chatsvc_logic::GroupMessageInfos>& list){
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

    //call migfm.`proc_V2GetGroupMsg`(10000,20001,10,0,0)
	os<<"call migfm.proc_V2GetGroupMsg("<<platform<<","
			<<groupid<<","<<count<<","<<from<<","<<msgid<<")";
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
			chatsvc_logic::GroupMessageInfos group_msg;
			if(rows[0]!=NULL)
				group_msg.set_platform(atoll(rows[0]));
			if(rows[1]!=NULL)
				group_msg.set_msgid(atoll(rows[1]));
			if(rows[2]!=NULL)
				group_msg.set_fid(atoll(rows[2]));
			if(rows[3]!=NULL)
				group_msg.set_groupid(atoll(rows[3]));
			if(rows[4]!=NULL)
				group_msg.set_nickname(rows[4]);
			if(rows[5]!=NULL)
				group_msg.set_head(rows[5]);
			if(rows[6]!=NULL)
				group_msg.set_message(rows[6]);
			if(rows[7]!=NULL)
				group_msg.set_lasttime(rows[7]);
			list.push_back(group_msg);
		}
		return true;
	}
	return false;
}

}
