#include "storage/db_storage.h"
#include "basic/basic_info.h"
#include "log/mig_log.h"
#include <mysql.h>
#include <assert.h>
#include <sstream>

namespace base_storage{

base_storage::DBStorageEngine*  
		MYSQLDB::mysql_engine_ = NULL;


bool MYSQLDB::Init(std::list<base::ConnAddr>& addrlist){
	mysql_engine_ = DBStorageEngine::Create(IMPL_MYSQL);
	assert(mysql_engine_);
	return mysql_engine_->Connections(addrlist);
}

bool MYSQLDB::GetUserInfo(const std::string& uid,base::UserInfo& usrinfo){
	bool r = false;
	std::stringstream sql;
	uint32 num;
	db_row_t* db_rows;
	MYSQL_ROW rows;
	//select usrid,sex,extadd,street,locality,region,pcode,
	//ctry,head,birthday,nickname from migfm_user_infos where usrid =10000
	sql<<"select usrid,username,sex,type,ctry,head,birthday,nickname,source "
		<<"from migfm_user_infos where usrid = '"<<uid.c_str()<<"';";

	MIG_DEBUG(USER_LEVEL,"sql[%s]\n",sql.str().c_str());
	r = mysql_engine_->SQLExec(sql.str().c_str());
	if(!r)
		return false;
	num = mysql_engine_->RecordCount();
	if(num>0){
		db_rows = mysql_engine_->FetchRows();
		rows = (*(MYSQL_ROW*)db_rows->proc); 
		//userid = rows[0];
		usrinfo.set_uid(rows[0]);
		usrinfo.set_username(rows[1]);
		usrinfo.set_sex(rows[2]);
		usrinfo.set_type(rows[3]);
		usrinfo.set_crty(rows[4]);
		usrinfo.set_head(rows[5]);
		usrinfo.set_birthday(rows[6]);
		usrinfo.set_nickname(rows[7]);
		usrinfo.set_source(rows[8]);
		return true;
	}
	return false;
}


}