#include "mysql_storage_impl.h"
#include "log/mig_log.h"
namespace base_storage{

MysqlStorageEngineImpl::MysqlStorageEngineImpl(){
    
    conn_.reset(new db_conn_t);
    result_.reset(new db_res_t);
    rows_.reset(new db_row_t);
    conn_.get()->proc = result_.get()->proc = rows_.get()->proc = NULL;
}

MysqlStorageEngineImpl::~MysqlStorageEngineImpl(){
}
	
bool MysqlStorageEngineImpl::Connections(std::list<base::ConnAddr>& addrlist){
    connected_ = false;
    base::ConnAddr addr;
    //MYSQL* mysql = (MYSQL*)conn_.get()->proc;
    MYSQL* mysql = NULL;
    mysql = mysql_init((MYSQL*)0);
    mysql_options(mysql,MYSQL_SET_CHARSET_NAME,"utf8");
    std::list<base::ConnAddr>::iterator it = addrlist.begin();
    while(it!=addrlist.end()){
        addr = (*it);
    	if(mysql_real_connect(mysql,addr.host().c_str(),
    		                  addr.usr().c_str(),addr.pwd().c_str(),
    		                  addr.source().c_str(),addr.port(),0,
    		                  /*CLIENT_INTERACTIVE*/CLIENT_MULTI_STATEMENTS)==NULL){
            MIG_ERROR(USER_LEVEL,"mysql:connection to database failed [%s]",
                      mysql_error(mysql));
    		return false;
    	}
        
        //mysql_set_character_set(&mysql,"gbk");
        MIG_INFO(USER_LEVEL,"mysql ip[%s] port[%d] user[%s] pwd[%s] db[%s]",
                addr.host().c_str(),addr.port(),addr.usr().c_str(),
                addr.pwd().c_str(),addr.source().c_str());
    	break;
    }
    mysql_query(mysql,"SET NAMES 'binary'");
    connected_ = true;
    conn_.get()->proc = mysql;
    return true;
}

bool MysqlStorageEngineImpl::Release(){
	MYSQL_RES * result = (MYSQL_RES *)result_.get()->proc;
	MYSQL* mysql = (MYSQL*)conn_.get()->proc;
    if(connected_){
	   if (result!=NULL){
		   mysql_free_result(result);
		   result_.get()->proc = NULL;
	   }
	   mysql_close(mysql);
	   MIG_DEBUG(USER_LEVEL,"MysqlStorageEngineImpl::Release()");
    }
    return true;
}

bool MysqlStorageEngineImpl::FreeRes(){
    MYSQL_RES * result = (MYSQL_RES *)result_.get()->proc;
    if(result){
        mysql_free_result(result);
        result_.get()->proc = NULL;
    }
    return true;
}

bool MysqlStorageEngineImpl::SQLExec(const char* sql){
    FreeRes();
    MYSQL* mysql = (MYSQL*)conn_.get()->proc;
    //MIG_DEBUG(USER_LEVEL,"SQLExec sql[%s]\n",sql);
    mysql_query(mysql,sql);
    result_.get()->proc = mysql_store_result(mysql);
	if (mysql->net.last_errno!=0){
		MIG_ERROR(USER_LEVEL,"mysql error code [%d] [%s]",
			mysql_errno(mysql),mysql_error(mysql));
	}
	//MIG_DEBUG(USER_LEVEL,"errno [%d]",mysql->net.last_errno);
// 	unsigned long ulCount = (unsigned long)mysql_num_rows((MYSQL_RES *)(result_.get()->proc));
// 	MIG_DEBUG(USER_LEVEL,"ulconut %d\n",ulCount);
    return true;
}

uint32 MysqlStorageEngineImpl::RecordCount(){
    unsigned long ulCount = (unsigned long)mysql_num_rows((MYSQL_RES *)(result_.get()->proc));
	MIG_DEBUG(USER_LEVEL,"ulconut ==%d===\n",ulCount);
	return ulCount;
}
bool MysqlStorageEngineImpl::Affected(unsigned long& rows){
	rows = (unsigned long)mysql_affected_rows((MYSQL*)conn_.get()->proc);
	return true;
}

db_row_t* MysqlStorageEngineImpl::FetchRows(){
	//MYSQL_ROW* row = (MYSQL_ROW*)(rows_.get()->proc);
    
	MYSQL_RES* result = (MYSQL_RES*)(result_.get()->proc);
	row_ = mysql_fetch_row(result);
    rows_.get()->proc = &row_;
	return rows_.get();
}

bool MysqlStorageEngineImpl::CheckConnect(void){
	MYSQL* mysql = (MYSQL*)conn_.get()->proc;
    return (mysql_ping(mysql)==0);
}


}
