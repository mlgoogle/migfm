#include "usr_mgr_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include <sstream>

#define		TIME_TEST		1025

namespace usr_logic{


UsrMgrEngine::UsrMgrEngine(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	usr_logic::ThreadKey::InitThreadKey();
	usr_logic::SomeUtils::InitRandom();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	storage::DBComm::Init(config->mysql_db_list_);
	storage::MemComm::Init(config->mem_list_);
	storage::RedisComm::Init(config->redis_list_);

}

UsrMgrEngine::~UsrMgrEngine(){
	ThreadKey::DeinitThreadKey ();
	SomeUtils::DeinitRandom ();
}

UsrMgrEngine* UsrMgrEngine::instance_ = NULL;
UsrMgrEngine* UsrMgrEngine::GetInstance(){
   if (instance_==NULL){
        instance_ = new UsrMgrEngine();
   }

   return instance_;
}

void UsrMgrEngine::FreeInstance(){
	delete instance_;
}

bool UsrMgrEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool UsrMgrEngine::OnBroadcastConnect(struct server *srv, int socket, 
									  void *data, int len){
    return true;
}

bool UsrMgrEngine::OnBroadcastMessage(struct server *srv, int socket, void *msg, int len){
    return true;
}

bool UsrMgrEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
    return true;
}

bool UsrMgrEngine::OnTimeout(struct server *srv, char *id, int opcode, int time){

	return true;
}

bool UsrMgrEngine::OnUsrMgrClose(struct server *srv, int socket){
    return true;
}

bool UsrMgrEngine::OnUsrMgrConnect(struct server *srv, int socket){
    return true;
}

bool UsrMgrEngine::OnUsrMgrMessage(struct server *srv, int socket, 
								   const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);

	if (type=="regedit"){
		RegeditUsr(socket,packet);
	}else if (type=="update"){
		UpdateUserinfo(socket,packet);
	}else if (type=="get"){
		GetUserInfo(socket,packet);
	}else if (type=="guest"){
		CreateGuest(socket,packet);
	}
    return true;
}


bool UsrMgrEngine::CreateGuest(const int socket, const packet::HttpPacket &packet){
	
	packet::HttpPacket pack = packet;
	std::stringstream os;
	std::stringstream os1;
	std::stringstream os2;
	std::string nickname;
	char* utf_nickname;
	size_t utf_nickname_size;
	std::string source = "0";
	int32 random_num = usr_logic::SomeUtils::GetRandomID();
	os<<random_num<<"@miglab.com";
	os1<<random_num;
	os2<<"游客"<<random_num;
	base::BasicUtil::GB2312ToUTF8(os2.str().c_str(),os2.str().length(),
		&utf_nickname,&utf_nickname_size);
	nickname.assign(utf_nickname,utf_nickname_size);

	RegeditUsr(socket,0,os.str(),os1.str(),nickname,source);

}

bool UsrMgrEngine::GetUserInfo(const int socket,const packet::HttpPacket& packet){
	std::string uid;
	packet::HttpPacket pack = packet;
	bool r = false;
	std::stringstream os;
	std::string username;
	std::string nickname;
	std::string source;
	std::string gender ;
	std::string type = "1";
	std::string birthday;
	std::string location;
	std::string head;
	std::string result_out;
	std::string status = "0";
	std::string msg = "0";
	std::string result;
	r = pack.GetAttrib(USERNAME,username);
	if (!r){
		LOG_ERROR("get uid error");
		return false;
	}
	r = storage::DBComm::GetUserInfos(username,uid,nickname,gender,type,
		birthday,location,source,head);
	if (!r){ //vailed.
		status = "0";
		msg = "获取信息失败";
		//msg = "1";
		usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
		LOG_DEBUG2("[%s]",result_out.c_str());
		usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
		return false;
	}
	status = "1";
	os<<"\"userid\":\""<<uid<<"\",\"username\":\""<<username.c_str()
		<<"\",\"nickname\":\""<<nickname.c_str()<<"\",\"gender\":\""
		<<gender.c_str()<<"\",\"type\":\""<<type.c_str()<<"\",\"birthday\":\""
		<<birthday.c_str()<<"\",\"location\":\""<<location.c_str()
		<<"\",\"source\":\""<<source.c_str()<<"\",\"head\":\""<<head.c_str()
		<<"\"";
	result = os.str();
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,0);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

bool UsrMgrEngine::UpdateUserinfo(const int socket,const packet::HttpPacket& packet){
    bool r = false;
	packet::HttpPacket pack = packet;
	std::string uid;
	std::string username;
	std::string password;
	std::string nickname;
	std::string source;
	std::string gender ;
	std::string type = "1";
	std::string birthday;
	std::string location;
	std::string head;
	std::string result_out;
	std::string status = "0";
	std::string msg = "0";
	std::string result;
	r = pack.GetAttrib(UID,uid);
	if (!r){
		LOG_ERROR("get uid error");
		return false;
	}

	r = pack.GetAttrib(USERNAME,username);
	if (!r){
		LOG_ERROR("get username error");
		return false;
	}

	r = pack.GetAttrib(NICKNAME,nickname);
	if (!r){
		LOG_ERROR("get nickname error");
		return false;
	}

	r = pack.GetAttrib(GENDER,gender);
	if (!r){
		LOG_ERROR("get gender error");
		return false;
	}

	r = pack.GetAttrib(BIRTHDAY,birthday);
	if (!r){
		LOG_ERROR("get birthday error");
		return false;
	}

	r = pack.GetAttrib(LOCATION,location);
	if (!r){
		LOG_ERROR("get location error");
		return false;
	}

	r = pack.GetAttrib(SOURCE,source);
	if (!r){
		LOG_ERROR("get source error");
		return false;
	}

	r = pack.GetAttrib(HEAD,head);
	if (!r){
		LOG_ERROR("get head error");
		return false;
	}

	int iuid = atoll(uid.c_str());
	r = storage::DBComm::UpDateUserInfos(atoll(uid.c_str()),username,nickname,gender,type,
		                                 birthday,location,source,head);
	if (!r){ //vailed.
		status = "0";
		msg = "更新失败";
		//msg = "1";
		usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
		LOG_DEBUG2("[%s]",result_out.c_str());
		usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
		return false;
	}
	status = "1";
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

bool UsrMgrEngine::RegeditUsr(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string username;
	std::string password;
	std::string nickname;
	std::string source;

	r = pack.GetAttrib(USERNAME,username);
	if (!r){
		LOG_ERROR("get username error");
		return false;
	}

	r = pack.GetAttrib(PASSWORD,password);
	if (!r){
		LOG_ERROR("get password error");
		return false;
	}

	r = pack.GetAttrib(NICKNAME,nickname);
	if (!r){
		LOG_ERROR("get nickname error");
		return false;
	}

	r = pack.GetAttrib(SOURCE,source);
	if (!r){
		LOG_ERROR("get source error");
		return false;
	}

	RegeditUsr(socket,1,username,password,nickname,source);

	return true;
}

bool UsrMgrEngine::RegeditUsr(const int socket, const int flag,const std::string username, 
							  const std::string password, const std::string nickname, 
							  const std::string source, const std::string gender /*= "1"*/, 
							  const std::string type/* = "1"*/, 
							  const std::string birthday/* = "1986-10-01"*/, 
							  const std::string location,const std::string head){
	  //check user Exist
	  bool r = false;
	  int64 uid  = 0;
	  std::string result_out;
	  std::string status = "0";
	  std::string msg = "0";
	  std::string result;
	  std::stringstream os;
	  char* utf_location = NULL;
	  size_t utf_location_size = 0;
	  std::string str_utf8_location;
	  if (flag){
		  r = storage::DBComm::GetUserIndent(username,uid);
		  if (r){//exist
			  //
			  status = "0";
			  msg = "账号已存在";
			  usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
			  usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
			  return false;
		  }
	  }

	  r = storage::DBComm::RegeditUser(username,password,nickname,source);
	  if (!r){//regedit
		  status = "0";
		  msg = "注册失败";
		  usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
		  LOG_DEBUG2("[%s]",result_out.c_str());
		  usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
		  return false;
	  }

	  r = storage::DBComm::GetUserIndent(username,uid);
	  if (!r){ //vailed.
		  status = "0";
		  msg = "账号不存在";
		  //msg = "1";
		  usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
		  LOG_DEBUG2("[%s]",result_out.c_str());
		  usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
		  return false;
	  }

	  base::BasicUtil::GB2312ToUTF8(location.c_str(),location.length(),
		  &utf_location,&utf_location_size);
	  str_utf8_location.assign(utf_location,utf_location_size);
	  LOG_DEBUG2("[%s]",str_utf8_location.c_str());
	  if (utf_location){
		  delete [] utf_location;
		  utf_location = NULL;
	  }

	  r = storage::DBComm::AddUserInfos(uid,username,nickname,gender,type,
		  birthday,str_utf8_location,source,head);
	  if (!r){//init error
		  status = "0";
		  msg = "更新用户信息失败";
		  usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
		  LOG_DEBUG2("[%s]",result_out);
		  usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
		  return false;
	  }

	  status = "1";
	  if (flag==0){
		  os<<"\"username\":\""<<username.c_str()<<"\",\"password\":\""
			  <<password.c_str()<<"\"";
		  result = os.str();
	  }

	  usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
	  LOG_DEBUG2("[%s]",result_out.c_str());
	  usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

void UsrMgrEngine::GetResultMsg(std::string &status, std::string &msg, 
							 std::string &result,std::string &out_str){
	 char* out;
	 size_t out_len;
	 std::stringstream os;
	 os<<"{\"status\":"<<status.c_str()<<",\"msg\":"
		 <<msg.c_str()<<",\"result\":{"<<result.c_str()
		 <<"}}";
	 base::BasicUtil::GB2312ToUTF8(os.str().c_str(),os.str().length(),
		 &out,&out_len);
	 out_str.assign(out,out_len);
	 LOG_DEBUG2("%s",out_str.c_str());
}
}