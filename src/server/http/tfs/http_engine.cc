#include "http_engine.h"
#include "config/config.h"
#include "http_response.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
#include <sstream>
#include <mysql.h>
namespace spider{
	
SpiderHttpEngineImpl::SpiderHttpEngineImpl(base_storage::DBStorageEngine* db_engine)
:db_engine_(db_engine)
,index_(0)
,port_(0){
    Init();
}

SpiderHttpEngineImpl::~SpiderHttpEngineImpl(){
}

int32 SpiderHttpEngineImpl::Init(){

    bool r = false;
    return 1;
}

int32 SpiderHttpEngineImpl::RequestHttp(const std::string& url,std::string& content){
	spider::HttpResponse*   http_ = NULL;
    bool r = false;
	int ret = 0;
    http_ = new spider::HttpResponse(url);
    if(http_==NULL)
     	return 0;
    http_->Get(host_,port_);
    r = http_->GetContent(content);
	if(!r)
		goto out;
	if (http_->GetCode()==403){
		MIG_ERROR(USER_LEVEL,"ip is blocked");
		//UpDateProxyHost();
		while ((ret=GetProxyHost())!=1){
			if (ret==-1){
				MIG_ERROR(USER_LEVEL,"no proxy ip used");
				break;
			}else if (ret==0){
				continue;
			}
		}
		goto out;
	}

    if(content.length()<10){
        MIG_DEBUG(USER_LEVEL,"content: %s",content.c_str());
        goto out;
    }
out:
	if (http_){
		delete http_;
		http_ = NULL;
	}
    return 1;
}

int32 SpiderHttpEngineImpl::DownLoadFile(const std::string &url, const std::string &local){
	bool r = false;
	int code;
	int file_size = 0;
	spider::HttpDownFile*   down_ = NULL;
	down_ = new spider::HttpDownFile(url);
	if (down_==NULL)
		return 0;
	r = down_->DownLoad(local,code,file_size);
	if(!r)
		return 0;
	if (code==403){
		MIG_ERROR(USER_LEVEL,"ip is blocked");
		assert(0);
		return 0;
	}
	if(code!=200){
		MIG_DEBUG(USER_LEVEL,"down failed code[%d]",code);
		return 0;
	}
	return file_size;
}

int32 SpiderHttpEngineImpl::UpDateProxyHost(){
	std::stringstream sql;
	sql<<"UPDATE migfm_prox_ip SET flag = 0 WHERE id = "<<index_;
	db_engine_->SQLExec(sql.str().c_str());
	return 1;
}

int32 SpiderHttpEngineImpl::GetProxyHost(){
	std::stringstream sql;
	index_++;
	sql<<"select proxy_ip,proxy_port from migfm_prox_ip where flag=1 and id="<<index_;
	MIG_INFO(USER_LEVEL,"%s",sql.str().c_str());
	db_engine_->SQLExec(sql.str().c_str());
	int num = db_engine_->RecordCount();
	MIG_INFO(USER_LEVEL,"num[%d]",num);
	if (num>0){
		base_storage::db_row_t* db_rows = db_engine_->FetchRows();
		MYSQL_ROW rows = (*(MYSQL_ROW*)db_rows->proc);
		host_.assign(rows[0]);
		port_ = atol(rows[1]);
		MIG_INFO(USER_LEVEL,"proxy[%s],port[%d]",host_.c_str(),port_);
	}else{
		sql.str("");
		sql<<"select count(*) from migfm_prox_ip";
		db_engine_->SQLExec(sql.str().c_str());
		base_storage::db_row_t* db_rows = db_engine_->FetchRows();
		//int num = db_engine_->RecordCount();
		MYSQL_ROW rows = (*(MYSQL_ROW*)db_rows->proc);
		int num = atol(rows[0]);
		if (num>0)
			return 0;
		return -1;
	}
	return 1;
} 

}
