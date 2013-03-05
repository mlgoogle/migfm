#include "storage/db_serialization.h"
#include <string>
#include <sstream>
#include <assert.h>
#include <mysql.h>
#include "basic/basic_info.h"
#include "basic/basic_util.h"
#include "storage/storage.h"
#include "log/mig_log.h"

namespace base_storage{

base_storage::DBStorageEngine*
	MysqlSerial::mysql_db_engine_ = NULL;

bool MysqlSerial::Init(std::list<base::ConnAddr>& addrlist){
	mysql_db_engine_ = DBStorageEngine::Create(IMPL_MYSQL);
    assert(mysql_db_engine_);
    return mysql_db_engine_->Connections(addrlist);
}

bool MysqlSerial::GetUserInfo(const int32 usr_id,std::string& username,int32& sex,std::string& extadd,
	                          std::string& street,std::string& locality,std::string& regin,
	                          int32& pcode,std::string& ctry,std::string& head,
	                          std::string& birthday,std::string& nickname){
    bool r = false;
    std::stringstream sql;
    uint32 num;
    db_row_t* db_rows;
    MYSQL_ROW rows;
    //select usrid,sex,extadd,street,locality,region,pcode,
    //ctry,head,birthday,nickname from migfm_user_infos where usrid =10000
    sql<<"select usrid,sex,extadd, street,locality,region,pcode,ctry,head,birthday,"
        <<"nickname,username from migfm_user_infos where usrid = "<<usr_id<<";";
    r = mysql_db_engine_->SQLExec(sql.str().c_str());
    if(!r)
    	return false;
    num = mysql_db_engine_->RecordCount();
    if(num>0){
    	db_rows = mysql_db_engine_->FetchRows();
    	rows = (*(MYSQL_ROW*)db_rows->proc); 
    	sex = atol(rows[1]);
    	extadd = rows[2];
    	street = rows[3];
    	locality = rows[4];
    	regin = rows[5];
    	pcode = atol(rows[6]);
    	ctry = rows[7];
    	head = rows[8];
    	birthday = rows[9];
    	nickname = rows[10];
        username = rows[11];
    	return true;
    }
    return false;
}


 bool MysqlSerial::SetMusicInfo(const std::string& name,
                                const std::string& phone,
                                const std::string& enter,
                                const std::string& music_info){
    bool r = false;
    std::stringstream sql;
    std::string current_time = base::BasicUtil::FormatCurrentTime().c_str();
    MIG_DEBUG(USER_LEVEL,"current time[%s]",current_time.c_str());
    sql<<"insert into migfm_user_record(usrname,phone,content,time,enter) value(\'"<<name.c_str()<<"\',\'"
        <<phone.c_str()<<"\',\'"<<music_info.c_str()<<"\',\'"<<current_time.c_str()<<"\',\'"
        <<enter.c_str()<<"\')";

    r = mysql_db_engine_->SQLExec(sql.str().c_str());
    return r;
}

#if defined (MIG_SSO)
bool MysqlSerial::CheckUserPassword(const char*username,const char* password){ 
    std::stringstream os;
    bool r = false;
    uint32 num;
    db_row_t* db_rows;
    MYSQL_ROW rows;
    char* db_idx = NULL;
    char* db_usr_name = NULL;
    char* db_pass_word = NULL;
    os<<"select usrid,username,passwd from migfm_identity where username='"<<username<<"'"; 
    r = mysql_db_engine_->SQLExec(os.str().c_str());
    if(!r){
        MIG_ERROR(USER_LEVEL,"sqlexec error");
        return r;
    }
    num = mysql_db_engine_->RecordCount();
    if(num>0){
        db_rows = mysql_db_engine_->FetchRows();
        rows = (*(MYSQL_ROW*)db_rows->proc);
        db_idx = rows[0];
        db_usr_name = rows[1];
        db_pass_word = rows[2];
        if((strcmp(username,db_usr_name)==0)&&
           (strcmp(db_pass_word,password)==0)){
            return true;
        }
    }

    return  false;
}

#endif
}
