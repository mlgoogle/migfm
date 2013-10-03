#include "db_comm.h"
#include "logic_comm.h"
#include "thread_handler.h"
#include <mysql.h>
#include <sstream>

namespace storage{

std::list<base::ConnAddr> DBComm::addrlist_;

void DBComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void DBComm::Dest(){

}

base_storage::DBStorageEngine* DBComm::GetConnection(){

	try{
		base_storage::DBStorageEngine* engine = usr_logic::ThreadKey::GetStorageDBConn();
		if (engine){
			if (!engine->CheckConnect()){
				LOG_ERROR("Database %s connection was broken");
				engine->Release();
				if (engine){
					delete engine;
					engine = NULL;
				}
			}else
				return engine;
		}

		engine = base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
		engine->Connections(addrlist_);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		usr_logic::ThreadKey::SetStorageDBConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool DBComm::GetUserIndent(const std::string& username,int64& uid){
	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r = false;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}
	os<<"select usrid from "<<IDENTITY<<" where username=\'"
		<<username.c_str()<<"\';";

	LOG_DEBUG2("[%s]",os.str().c_str());

	r = engine->SQLExec(os.str().c_str());

	if (!r){
		LOG_ERROR2("exec sql error");
		return false;
	}
	int32 num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
		    uid = atoll(rows[0]);
		}
		return true;
	}
	return false;
}

bool DBComm::GetUserInfos(const std::string& username,std::string& uid,
						  std::string& nickname,std::string& gender,
						  std::string& type,std::string& birthday, 
						  std::string& location,std::string& source,
						  std::string& head){
	  base_storage::DBStorageEngine* engine = GetConnection();
	  std::stringstream os;
	  bool r = false;
	  MYSQL_ROW rows;
	  if (engine==NULL){
		  LOG_ERROR("GetConnection Error");
		  return false;
	  }
	  os<<"select usrid,sex,type,ctry,birthday,nickname,source,head from "
		  <<USERINFOS<<" where username=\'"<<username.c_str()<<"\';";
	  const char* sql = os.str().c_str();
	  LOG_DEBUG2("[%s]",os.str().c_str());
	  r = engine->SQLExec(os.str().c_str());

	  if (!r){
		  LOG_ERROR2("exec sql error");
		  return false;
	  }
	  int32 num = engine->RecordCount();
	  if(num>0){
		  while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			  //uid = atol(rows[0]);
			  uid = rows[0];
			  gender = rows[1];
			  type = rows[2];
			  location = rows[3];
			  birthday = rows[4];
			  nickname = rows[5];
			  source = rows[6];
			  head = rows[7];
		  }
		  return true;
	  }
	  return false;

}

bool DBComm::UpDateUserInfos(const int uid,const std::string& username, 
							 const std::string& nickname,const std::string& gender, 
							 const std::string& type,const std::string& birthday, 
							 const std::string& location, const std::string& source, 
							 const std::string& head){
	 base_storage::DBStorageEngine* engine = GetConnection();
	 std::stringstream os;
	 bool r = false;
	 if (engine==NULL){
		 LOG_ERROR("GetConnection Error");
		 return false;
	 }
	 os<<"update "<<USERINFOS<<" set username=\'"<<username.c_str()<<"\',sex=\'"
		 <<gender.c_str()<<"\',type=\'"<<type.c_str()<<"\',ctry=\'"<<location.c_str()
		 <<"\',birthday=\'"<<birthday.c_str()<<"\',nickname=\'"<<nickname.c_str()
		 <<"\',source=\'"<<source.c_str()<<"\',head=\'"<<head.c_str()<<"\' where usrid="
		 <<uid<<";";
	 LOG_DEBUG2("[%s]",os.str().c_str());
	 r = engine->SQLExec(os.str().c_str());

	 if (!r){
		 LOG_ERROR2("exec sql error");
	 }
	 return true;
}

bool DBComm::AddUserInfos(const int uid,const std::string& username,
						  const std::string& nickname, const std::string& gender,
						  const std::string& type, const std::string& birthday,
						  const std::string& location, const std::string& source,
						  const std::string& head){
	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r = false;
	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}
	LOG_DEBUG2("%s",location.c_str());
	os<<"insert into "<<USERINFOS<<"(usrid,username,sex,type,ctry,birthday,nickname,source"
		<<",head)value("<<uid<<",\'"<<username.c_str()<<"\',\'"<<gender.c_str()<<"\',\'"
		<<type.c_str()<<"\',\'"<<location.c_str()<<"\',\'"<<birthday.c_str()<<"\',\'"
		<<nickname.c_str()<<"\',\'"<<source.c_str()<<"\',\'"<<head.c_str()<<"\');";

	LOG_DEBUG2("[%s]",os.str().c_str());
	r = engine->SQLExec(os.str().c_str());

	if (!r){
		LOG_ERROR2("exec sql error");
	}
	return r;

}

bool DBComm::RegeditUser(const std::string &username, const std::string &password, 
						 const std::string &nickname, const std::string &source){
    base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r = false;
	std::string current_time;
	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}
	usr_logic::SomeUtils::GetCurrntTimeFormat(current_time);

	os<<"insert into "<<IDENTITY<<"(username,passwd,source,regtime)value(\'"
		<<username.c_str()<<"\',\'"<<password.c_str()<<"\',\'"<<
		source.c_str()<<"\',\'"<<current_time.c_str()<<"\');";

	LOG_DEBUG2("[%s]",os.str().c_str());
	r = engine->SQLExec(os.str().c_str());

	if (!r){
		LOG_ERROR2("exec sql error");
	}
	return r;
}

bool DBComm::CheckUserInfo(const std::string& clientid,const std::string& token,
						   const std::string& username,const std::string& password, 
						   std::string& uid,std::string& nickname, std::string& gender,
						   std::string& type, std::string& birthday,
						   std::string& location,std::string& source,
						   std::string& head,int& return_code){

	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r =false;
	MYSQL_ROW rows;
	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return false;
	}
	//call proc_CheckUser('1','eweqwewqe','988965442@miglab.com','988965442',@usrid,@sex,@type,@ctry,@birthday,@head,@username,@nickname,@return_code);
	os<<"call proc_CheckUser(\'"
		<<clientid.c_str()<<"\',\'"<<token.c_str()
		<<"\',\'"<<username.c_str()<<"\',\'"
		<<password.c_str()<<"\',"
		<<"@usrid,@sex,@type,@ctry,@birthday,"
		<<"@head,@username,@nickname,@return_code);";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	//获取值
	os.str("");
	sql.c_str();
	os<<"select @usrid,@sex,@type,@ctry,@birthday,"
		<<"@head,@username,@nickname,@return_code,@return_str";
	sql = os.str();
	r = engine->SQLExec(sql.c_str());
	LOG_DEBUG2("[%s]", sql.c_str());
	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}

	int num = engine->RecordCount();
	if (num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
			return_code = atoi(rows[8]);
			if (return_code!=0){
				LOG_ERROR2("error code [%s]",rows[9]);
				return false;
			}
			uid = rows[0];
			gender = rows[1];
			type = rows[2];
			location = rows[3];
			birthday = rows[4];
			head = rows[5];
			nickname = rows[7];
		}
	}
	return true;
}

bool DBComm::RegistUser(const char* plat_id, const char* plat_session, 
						const char* password, int &sex, std::string &username, 
						std::string &nickname, int64 &userid, int64& type, 
						std::string &location, std::string &birthday, 
						std::string &head){
	base_storage::DBStorageEngine* engine = GetConnection();
	std::stringstream os;
	bool r = false;
	int return_code;
	MYSQL_ROW rows;
	if (engine == NULL) {
		LOG_ERROR("GetConnection Error");
		return false;
	}

	//call proc_RegisterUser('2','eweqwewqe','dasd',1,'flaght','oldk','北京','1986-09-03','http://fm.miglab.com/1.jpg',@usrid,@sex,@type,@ctry,@birthday,@head,@username,@nickname,@return_code,@return_str);

    os<<"call proc_RegisterUser(\'"
	  <<plat_id<<"\',\'"<<plat_session
	  <<"\',\'"<<password<<"\',"
	  <<sex<<",\'"<<username.c_str()<<"\',\'"
	  <<nickname.c_str()<<"\',\'"<<location.c_str()<<"\',\'"
	  <<birthday.c_str()<<"\',\'"<<head.c_str()<<"\',"
	  <<"@usrid,@sex,@type,@ctry,@birthday,@head,@username,@nickname,@return_code,@return_str);";
	std::string sql = os.str();
	LOG_DEBUG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}
	//获取值
	os.str("");
	sql.c_str();
	os<<"select @usrid,@sex,@type,@ctry,@birthday,"
		<<"@head,@username,@nickname,@return_code,@return_str";
	sql = os.str();
	r = engine->SQLExec(sql.c_str());
	LOG_DEBUG2("[%s]", sql.c_str());
	if (!r) {
		LOG_ERROR2("exec sql error");
		return false;
	}

	int num = engine->RecordCount();
	if (num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)) {
			return_code = atoi(rows[8]);
			if (return_code!=0){
				LOG_ERROR2("error code [%s]",rows[9]);
				return false;
			}
			userid = atoll(rows[0]);
			sex = atol(rows[1]);
			type = atoll(rows[2]);
			location = rows[3];
			birthday = rows[4];
			head = rows[5];
			username = rows[6];
			nickname = rows[7];
		}
	}
	return true;
}

}
