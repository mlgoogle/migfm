#include "usr_mgr_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "intertface/robot_interface.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "basic/errno_comm.h"
#include "config/config.h"
#include "basic/radom_in.h"
#include "json/json.h"
#include "storage/dic_storage.h"
#include "pushmsg/push_connector.h"
#include <sstream>

#define		TIME_TEST		1025

namespace usr_logic{


UsrMgrEngine::UsrMgrEngine()
:robot_server_socket_(0){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	usr_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	storage::DBComm::Init(config->mysql_db_list_);
	storage::MemComm::Init(config->mem_list_);
	storage::RedisComm::Init(config->redis_list_);
	base_storage::MemDic::Init(config->mem_list_);
	base::SysRadom::GetInstance();

	base_push::PushConnectorEngine::Create(base_push::IMPL_BAIDU);
	base_push::PushConnectorEngine::GetPushConnectorEngine()->Init(config->mysql_db_list_);

	base_lbs::LbsConnectorEngine::Create(IMPL_BAIDU);
	base_lbs::LbsConnectorEngine::GetLbsConnectorEngine()->Init(config->mysql_db_list_);
}

UsrMgrEngine::~UsrMgrEngine(){
	ThreadKey::DeinitThreadKey ();
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
	robot_server_socket_ = socket;
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
		//RegeditUsr(socket,packet);
		RegistUser(socket,packet);
	}else if (type=="update"){
		UpdateUserinfoBrief(socket,packet);
	}else if (type=="get"){
		GetUserInfo(socket,packet);
	}else if (type=="guest"){
		CreateGuest(socket,packet);
	}else if (type=="login"){
		UserLogin(socket,packet); 
	}else if (type=="bdbindpush"){
		UserPushBind(socket,packet);
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
	int32 random_num = base::SysRadom::GetInstance()->GetRandomID();
	os<<random_num<<"@miglab.com";
	os1<<random_num;
	os2<<"游客"<<random_num;
	base::BasicUtil::GB2312ToUTF8(os2.str().c_str(),os2.str().length(),
		&utf_nickname,&utf_nickname_size);
	nickname.assign(utf_nickname,utf_nickname_size);

	CreateGuest(socket,0,os.str(),os1.str(),utf_nickname,source,"1","0");

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

bool UsrMgrEngine::UpdateUserinfoBrief(const int socket,const packet::HttpPacket& packet){
    bool r = false;
	packet::HttpPacket pack = packet;
	std::string uid;
	std::string nickname;
	std::string gender;
	std::string birthday;
	std::string result_out;
	std::string status = "0";
	std::string msg = "0";
	std::string result;

	r = pack.GetAttrib(UID,uid);
	if (!r){
		LOG_ERROR("get uid error");
		return false;
	}


	r = pack.GetAttrib(NICKNAME,nickname);
	if (!r){
		LOG_ERROR("get nickname error");
		goto ret;
		return false;
	}

	r = pack.GetAttrib(GENDER,gender);
	if (!r){
		LOG_ERROR("get gender error");
		goto ret;
		return false;
	}

	r = pack.GetAttrib(BIRTHDAY,birthday);
	if (!r){
		LOG_ERROR("get birthday error");
		goto ret;
		return false;
	}

	r = storage::DBComm::UpdateUserInfos(atoll(uid.c_str()),nickname,gender,
		                                 birthday);
	storage::MemComm::DelUserinfo(uid);
ret:
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


bool UsrMgrEngine::RegistUser(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string result;
	std::string result_out;
	std::string status;
	std::string msg;


	Json::Reader reader;
	Json::Value  root;
	Json::Value& result = root["result"];
	std::string username;
	std::string password;
	std::string nickname;
	std::string source;
	std::string session;
	std::string s_sex;
	std::string location;
	std::string birthday;
	std::string head;
	std::string token;
	std::string latitude;
	std::string longitude;
	std::string address;
	std::string city;
	std::string district;
	std::string province;
	std::string street;
	std::stringstream ssuid;

	int err;

	int64 usrid;
	int sex;
	int64 type;
	int64 new_msg_num = 0;
	int32 utf8_flag = 0;
	std::stringstream os;
	int32 return_code = 0;
#if PACKAGE_HTTP_API
	r = base_net::MYHttpApi::OnUserRegister(pack,username,password,nickname,source,
			session,s_sex,birthday,location,address,head,int& err);
	if(!r){
		status = "0";
		msg = http_strerror(err);
		goto ret;
	}
	//通过IP转换城市
	if(location.empty()){
	base_lbs::LbsConnectorEngine::GetLbsConnectorEngine()->IPtoAddress(address,latitude,longitude,
			location,district,province,street);
	}
#else
	r = pack.GetAttrib(SOURCE,source);
	if (!r){
		status = "0";
		msg = migfm_strerror(MIG_FM_SOURCE_NO_VAILED);
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(SESSION,session);
	if (!r){
		if(source!="0"){
			status = "0";
			msg = migfm_strerror(MIG_FM_SOURCE_SESSION_NO_VAILED);
			utf8_flag = 0;
			goto ret;
		}else{
			session = "1";
		}
	}

	//第三方平卿用户名为秿密码为空 设置默认用户县密码 
	//本平卿昵称为空

	r = pack.GetAttrib(USERNAME,username);
	if (!r){
		if(source=="0"){
			status = "0";
			msg = migfm_strerror(MIG_FM_USERNAME_NO_VAILED);
			utf8_flag = 0;
			goto ret;
		}
		else{
			username = "default@miglab.com";
		}
	}

	r = pack.GetAttrib(PASSWORD,password);
	if (!r){
		if(source=="0"){
			status = "0";
			msg = migfm_strerror(MIG_FM_PASSWORD_NO_VAILED);
			utf8_flag = 0;
			goto ret;
		}
		else{
			password = "123456";
		}
	}

	r = pack.GetAttrib(NICKNAME,nickname);
	if (!r){
		nickname = "米格用户";
	}
	//性别
	r = pack.GetAttrib(SEX,s_sex);
	if(!r){
		if(source=="0"){
			s_sex = "1";
		}
	}
	sex = atoi(s_sex.c_str());

	//生日
	r = pack.GetAttrib(BIRTHDAY,birthday);
	if(!r){
		if(source=="0"){
			birthday = "1986-10-01";
		}
	}

	//地区
	r = pack.GetAttrib(LOCATION,location);
	if(!r){
		if(source=="0"){
			location = "浙江杭州";
		}
	}

	//头像
	r = pack.GetAttrib(HEAD,head);
	if(!r){
		if(source=="0"){
			head = "http://fm.miglab.com/default.jpg";
		}
	}
#endif

	r = storage::DBComm::RegistUser(source.c_str(),session.c_str(),
		           password.c_str(),sex,username,nickname,usrid,
				   type,location,birthday,head,return_code);


	if (!r){//用户存在
		status = "0";
		msg = migfm_strerror(MIG_FM_USER_EXITS);
		utf8_flag = 0;
		goto ret;
	}

	//写入默认消息
	if(return_code==0&&atol(source.c_str())>=0)
		RegeditDefaultMsg(usrid);
	//获取token
	ssuid<<usrid;
	base::BasicUtil::GetUserToken(ssuid.str(),token);
	//获取最新消息
	storage::RedisComm::GetNewMsgNum(usrid,new_msg_num);


	os<<"\"userid\":\""<<usrid<<"\",\"username\":\""<<username.c_str()
		<<"\",\"nickname\":\""<<nickname.c_str()<<"\",\"gender\":\""<<sex
		<<"\",\"type\":\""<<type<<"\",\"birthday\":\""<<birthday.c_str()
		<<"\",\"location\":\""<<location.c_str()<<"\",\"age\":27,\"head\":\""
		<<head.c_str()<<"\","<<"\"token\":\""<<token.c_str()<<"\",\"new_msg_num\":"
		<<new_msg_num;



	result = os.str();
	status = "1";
	//通知机器人登陆
	NoticeUserLogin(robot_server_socket_,10000,usrid,0,0);

ret:

	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
}

bool UsrMgrEngine::UserPushBind(const int socket,const packet::HttpPacket& packet){
	Json::Value root;
	int32 err;
	std::string err_str;
	Json::FastWriter wr;
	packet::HttpPacket pack = packet;

	bool r = base_push::PushConnectorEngine::GetPushConnectorEngine()->BindPushUserinfo(pack,err,err_str);
	if(!r){
		root["msg"] = err_str;
		root["status"] = 0;
	}else{
		root["status"] = 1;
	}
	std::string res = wr.write(root);
	usr_logic::SomeUtils::SendFull(socket,res.c_str(),res.length());
	return true;
}

bool UsrMgrEngine::UserLogin(const int socket,const packet::HttpPacket& packet){
	bool  r = false;
	std::stringstream os;
	packet::HttpPacket pack = packet;
	std::string clientid;
	std::string token;
	std::string username;
	std::string password;
	std::string userid;
	std::string nickname;
	std::string status;
	std::string msg;
	std::string result;
	std::string source;
	std::string session;
	std::string s_sex;
	std::string location;
	std::string birthday;
	std::string head;
	std::string type;
	int32 return_code;
	int64 new_msg_num = 0;
	std::string result_out;
	int32 utf8_flag = 0;

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

	//check 
	r = storage::DBComm::CheckUserInfo(clientid,token,username,password,userid,nickname,s_sex,type,birthday,location,source,head,return_code);
	if (r&&(return_code==0)){
		//获取token
		base::BasicUtil::GetUserToken(userid,token);
		//获取最新消息
		storage::RedisComm::GetNewMsgNum(atoll(userid.c_str()),new_msg_num);

		os<<"\"userid\":\""<<userid<<"\",\"username\":\""<<username.c_str()
			<<"\",\"nickname\":\""<<nickname.c_str()<<"\",\"gender\":\""<<s_sex
			<<"\",\"type\":\""<<type<<"\",\"birthday\":\""<<birthday.c_str()
			<<"\",\"location\":\""<<location.c_str()<<"\",\"age\":27,\"head\":\""
			<<head.c_str()<<"\","<<"\"token\":\""<<token.c_str()<<"\",\"new_msg_num\":"
			<<new_msg_num;
		result = os.str();
		status = "1";
		//通知机器人登陆
		NoticeUserLogin(robot_server_socket_,10000,atoll(userid.c_str()),0,0);
	}else{
		//错误判断
		switch (return_code){
			case 1:
				msg = migfm_strerror(MIG_FM_HTTP_PLAT_INVALID);
				break;
			case 2:
				msg = migfm_strerror(MIG_FM_HTTP_USERCHECK_INVALID);
				break;
			case 3:
				msg = migfm_strerror(MIG_FM_HTTP_USERINFO_INVALID);
				break;
		}
		status = "0";
	}
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
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

	r = pack.GetAttrib(SOURCE,source);
	if (!r){
		LOG_ERROR("get source error");
		return false;
	}

	//RegeditUsr(socket,1,username,password,nickname,source);
	CreateGuest(socket,1,username,password,nickname,source);
	return true;
}

bool UsrMgrEngine::CreateGuest(const int socket,const int flag,const std::string username, 
							   const std::string password,const std::string nickname, 
							   const std::string source,const std::string gender /* =  */, 
							   const std::string type /* =  */,const std::string birthday /* =  */,
							   const std::string location /* =  */, const std::string head /* = */ ){
	  
	   std::string result_out;
	   std::string status = "0";
	   std::string msg = "0";
	   std::string result;
	   std::stringstream os;
	   os<<"\"username\":\""<<username.c_str()<<"\",\"password\":\""
		   <<password.c_str()<<"\"";
	   result = os.str();
	   usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out);
	   LOG_DEBUG2("[%s]",result_out.c_str());
	   usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
    
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
			  msg = "账号已存";
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
		  msg = "账号不存";
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
		  msg = "更新用户信息失败失败失败！！";
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

void UsrMgrEngine::RegeditDefaultMsg(const int64 uid){
	int64 msg_id;
	bool r = storage::RedisComm::GenaratePushMsgID(uid,msg_id);
	//
	storage::DBComm::RecordDefaultMessage(uid,msg_id);
}

}
