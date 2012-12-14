#include "mysql_storage_impl.h"
#include "mig_log.h"
#include "mysql_operator_info.h"
#define MYSQL_ERROR(str)\
    if(mysql_.net.last_error!=0){\
    	MIG_ERROR(USER_LEVEL,"%s failed %s",str,mysql_error(&mysql_));\
    	if(connected_)\
    		mysql_close(&mysql_);\
    	return false;\
    }\
    

namespace base{
	
MysqlStorageEngineImpl::MysqlStorageEngineImpl(){

}

MysqlStorageEngineImpl::~MysqlStorageEngineImpl(){

}

bool MysqlStorageEngineImpl::Connections(std::list<ConnAddr>& addrlist){
	return logic_.Connections(addrlist);
}

bool MysqlStorageEngineImpl::Release(){
	return logic_.Release();
}

bool MysqlStorageEngineImpl::SetValue(const char* key,const size_t key_len,const char* val,
										const size_t val_len,struct BaseStorage* base){

    return true;
}

bool MysqlStorageEngineImpl::AddValue(const char* key,const size_t key_len,const char* val,const size_t val_len,
									  struct BaseStorage* base){
	
	int opcode = base->opcode_;
	switch(opcode){
		case ADD_USER_INFO:{
			UserInfo* usrinfo = static_cast<UserInfo*>(const_cast<BaseStorage*>(base));
			logic_.AddUserInfo(usrinfo->id_,usrinfo->usrname_,usrinfo->password_,
					 			usrinfo->sex_,usrinfo->address_,usrinfo->name_,
					 			usrinfo->id_cards_,usrinfo->head_);
			break;
		}
	}
    return true;	 
}

bool MysqlStorageEngineImpl::ReplaceValue(const char* key,const size_t key_len,const char*val,const size_t val_len,
											struct BaseStorage* base){
	return true;
}

bool MysqlStorageEngineImpl::GetValue(const char* key,const size_t key_len,char** val,size_t* val_len,struct BaseStorage** base){
	
	int opcode = (*base)->opcode_;
	switch(opcode){
		case GET_USER_INFO:
			{
				UserInfo* usrinfo = static_cast<UserInfo*>((*base));
				logic_.GetUserInfo(key,&usrinfo);
			}
		break;
	}
	return true;
}

bool MysqlStorageEngineImpl::DelValue(const char* key,const size_t key_len){
	return true;
}

bool MysqlStorageEngineImpl::MGetValue(const char* const * key_array,const size_t *key_len_array,size_t element_count){
	return true;
}

bool MysqlStorageEngineImpl::FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len){
	return true;
}

bool MysqlStorageEngineImpl::AddListElement(const char* key,const size_t key_len,
										const char* val,const size_t val_len){
	return true;
}

bool MysqlStorageEngineImpl::GetListElement (const char* key,const size_t key_len,
										const int index,char** val,size_t *val_len){
	return true;								
}

bool MysqlStorageEngineImpl::DelListElement(const char* key,const size_t key_len,const int index){
	return true;
}

bool MysqlStorageEngineImpl::SetListElement(const int index,const char* key,const size_t key_len,
										const char* val,const size_t val_len){
	return true;
}

bool MysqlStorageEngineImpl::GetListAll(const char* key,const size_t key_len,std::list<std::string>& list){
	return true;
}


/////////////////////////////////////////////////////////////////////
MysqlDB::MysqlDB(){
    Init();
}

MysqlDB::~MysqlDB(){
   Close();
}

bool MysqlDB::Init(){
	result_ = NULL;
	mysql_ = *mysql_init((MYSQL*)0);
}

bool MysqlDB::Connect(const std::string& host,const int port,
					  const std::string& user,const std::string& passwd,
					  const std::string& db){
    
    connected_ = false;
    mysql_options(&mysql_,MYSQL_SET_CHARSET_NAME,"utf8");
    if(mysql_real_connect(&mysql_,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),
    	port,0,CLIENT_INTERACTIVE)==NULL){
        MIG_ERROR(USER_LEVEL,"mysql:connection to database failed %s",mysql_error(&mysql_));
        return false;
    }
    
    mysql_query(&mysql_,"SET NAMES 'binary'");
    //MYSQL_ERROR("set binary");
    connected_ = true;
    return true;
}


bool MysqlDB::Close(){
    return SqlClose();
}

bool MysqlDB::SqlExec(std::string& sql){
    SqlClose();
    mysql_query(&mysql_,sql.c_str());
    result_ = mysql_store_result(&mysql_);
    return true;
    
}

bool MysqlDB::AffectedRows(unsigned long& rows){
	rows = (unsigned long)mysql_affected_rows(&mysql_);
	MYSQL_ERROR("affect rows");
	return true;
}

bool MysqlDB::RecordCount(unsigned long& count){
	count = (unsigned long)mysql_num_rows(result_);
	MYSQL_ERROR("record count");
	return true;
}

bool MysqlDB::FetchRows(MYSQL_ROW& row){
	row =mysql_fetch_row(result_);
	MYSQL_ERROR("fectch rows");
	return true;
}

bool MysqlDB::SqlClose(){
    if(result_&&connected_){
        mysql_free_result(result_);
        result_ = NULL;
    }
    return true;
}

bool MysqlDB::CheckConnect(void){
   return (mysql_ping(&mysql_)==0);
}

//////////////////////////////////////////////////////////////////////////


bool MysqlLogic::Connections(std::list<ConnAddr>& addrlist){
	ConnAddr addr;
	std::list<ConnAddr>::iterator it = addrlist.begin();
    while(it!=addrlist.end()){
    	addr = (*it);
    	mysql_.Connect(addr.host(),addr.port(),addr.usr(),addr.pwd(),addr.source());
    	++it;
    }
}

bool MysqlLogic::Release(){
	mysql_.Close();
}

bool MysqlLogic::AddUserInfo(const std::string& id,const std::string& usrname,const std::string& password,
					 		const int sex,const int address,const std::string& name,
					 		const std::string& id_cards,const std::string& head){

	std::stringstream sql;
	std::string s;
	std::stringstream ss_sex;
	std::stringstream ss_address;
	ss_sex<<sex;
	ss_address<<address;
	sql<<"insert into " <<TABLE_USERINFO
		<<"(id,usrname,password,sex,address,name,id_cards,head) values ("
		<<"'"<<id.c_str()<<"','"<<usrname.c_str()<<"','"<<password.c_str()<<"','"
		<<"','"<<ss_sex.str().c_str()<<ss_address.str().c_str()<<"','"
		<<name.c_str()<<"','"<<id_cards.c_str()<<"','"<<head.c_str()<<"')";
	s = sql.str();
	mysql_.SqlExec(s);
	return true;
}

bool MysqlLogic::GetUserInfo(const char* id,struct UserInfo** usrinfo){
	std::string s;
	std::stringstream sql;	
	sql<<"select(id,usrname,sex,address,name,id_cards,head,birthday) "<<TABLE_USERINFO
		<<" where id = '"<<id<<"';";
	s = sql.str();
	mysql_.SqlExec(s);
	
}


}
