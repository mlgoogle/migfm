#include "user_logic.h"
#include "user_basic_info.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic/logic_unit.h"
#include "logic/logic_infos.h"
#include "lbs/location_server.h"
#include "pushmsg/basic_push_info.h"
#include "pushmsg/push_connector.h"
#include "basic/scoped_ptr.h"
#include "basic/native_library.h"
#include "logic/logic_comm.h"
#include "intertface/robot_interface.h"
#include "config/config.h"
#include "common.h"
#include<dlfcn.h>

namespace usersvc_logic{

Userlogic*
Userlogic::instance_=NULL;

Userlogic::Userlogic(){
   if(!Init())
      assert(0);
}

Userlogic::~Userlogic(){
	//base_lbs::LbsConnectorEngine::FreeLbsConnectorEngine();
	base_lbs::LocationServer::Dest();
	base_push::PushConnectorEngine::FreePushConnectorEngine();
	usersvc_logic::DBComm::Dest();
	usersvc_logic::UserDicComm::Dest();
}

bool Userlogic::Init(){
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}
	r = config->LoadConfig(path);

	base_lbs::LocationServer::Init(config->mysql_db_list_,config->mem_list_);
	usersvc_logic::DBComm::Init(config->mysql_db_list_);
	usersvc_logic::UserDicComm::Init(config->redis_list_);
	base_push::PushConnectorEngine::Create(base_push::IMPL_BAIDU);
	base_push::PushConnectorEngine::GetPushConnectorEngine()->Init(config->mysql_db_list_);


	basic::libhandle  handle_lancher = NULL;
	handle_lancher = basic::load_native_library("./data.so");
	if (handle_lancher==NULL){
		MIG_ERROR(USER_LEVEL,"Can't load path data.so\n");
	}

	base_logic::DataEngine* (*pengine) (void);
	pengine = (base_logic::DataEngine *(*)(void))basic::get_function_pointer(handle_lancher, "GetDateEngine");
	if(pengine==NULL){
		MIG_ERROR(USER_LEVEL,"Can't find GetDateEngine\n");
	}

	data_engine_ = (*pengine)();
	/*int32 batch_uid_size = 4;
	int64* batch_uid = new int64[batch_uid_size];
	batch_uid[0] = 10283;
	batch_uid[1] = 10284;
	batch_uid[2] = 10285;
	batch_uid[3] = 10286;
	base_logic::UserInfo* info  = new base_logic::UserInfo[batch_uid_size];
	int32 out_batch_uid_size =0;
	data_engine_->BatchGetUserInfo(batch_uid,&batch_uid_size,info,&out_batch_uid_size);
	*/

	/*base_logic::UserInfo info;
	int64 uid = 10149;
	data_engine_->GetUserInfo(uid,info);*/


	/*base_logic::UserInfo info;
	info.set_uid(10149);
	info.set_nickname("kerry");
	info.set_city("杭州");
	//engine->DelUserInfo(10149);
	engine->SetUserInfo(10149,info);*/
    return true;
}

Userlogic*
Userlogic::GetInstance(){

    if(instance_==NULL)
        instance_ = new Userlogic();

    return instance_;
}



void Userlogic::FreeInstance(){
    delete instance_;
    instance_ = NULL;
}

bool Userlogic::OnUserConnect(struct server *srv,const int socket){

    return true;
}



bool Userlogic::OnUserMessage(struct server *srv, const int socket, const void *msg,const int len){

	const char* packet_stream = (char*)(msg);
	std::string http_str(packet_stream,len);
	std::string error_str;
	int error_code = 0;

	LOG_DEBUG2("%s",packet_stream);

	scoped_ptr<base_logic::ValueSerializer> serializer(base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP,&http_str));


	netcomm_recv::NetBase*  value = (netcomm_recv::NetBase*)(serializer.get()->Deserialize(&error_code,&error_str));
	if(value==NULL){
		error_code = STRUCT_ERROR;
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	scoped_ptr<netcomm_recv::HeadPacket> packet(new netcomm_recv::HeadPacket(value));
	int32 type = packet->GetType();
	switch(type){
		case QUICK_LOGIN:
			OnQuickLogin(srv,socket,value);
			break;
		case THIRID_LOGIN:
			OnThirdLogin(srv,socket,value);
			break;
		case BD_BIND_PUSH:
			OnBDBindPush(srv,socket,value);
			break;
		case LOGIN_RECORD:
			OnLoginRecord(srv,socket,value);
			break;
		case UPDATE_USERINFO:
			OnUpdateUserInfo(srv,socket,value);
			break;

	}

	return true;
}

bool Userlogic::OnUserClose(struct server *srv,const int socket){

    return true;
}



bool Userlogic::OnBroadcastConnect(struct server *srv, const int socket, const void *msg,const int len){
	robot_server_socket_ = socket;
    return true;
}

bool Userlogic::OnBroadcastMessage(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}



bool Userlogic::OnBroadcastClose(struct server *srv, const int socket){

    return true;
}

bool Userlogic::OnIniTimer(struct server *srv){

    return true;
}



bool Userlogic::OnTimeout(struct server *srv, char *id, int opcode, int time){

    return true;
}


bool Userlogic::OnQuickLogin(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg,const int len){
	scoped_ptr<netcomm_recv::QucikLogin> login(new netcomm_recv::QucikLogin(netbase));
	scoped_ptr<base_logic::LBSInfos> lbs_info;
	usersvc_logic::UserInfo userinfo;
	std::string token;
	bool r = false;
	int error_code = login->GetResult();
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}

	//获取地址
	lbs_info.reset(base_logic::LogicUnit::GetGeocderAndAddress((netcomm_recv::HeadPacket*)login.get()));
	if(r)
		login.get()->Reset();
	userinfo.set_imei(login->imei());
	userinfo.set_machine(login->machine());
	//存储用户信息
	usersvc_logic::DBComm::OnQuickRegister(userinfo,lbs_info.get());
	base_logic::LogicUnit::CreateToken(userinfo.uid(),token);
	scoped_ptr<netcomm_send::Login> qlogin(new netcomm_send::Login());


	qlogin->set_userinfo_address(userinfo.city());
	qlogin->set_userinfo_token(token);
	qlogin->set_userinfo_uid(userinfo.uid());
	qlogin->set_userinfo_nickname(userinfo.nickname());
	qlogin->set_userinfo_source(userinfo.type());
	qlogin->set_useromfo_head(userinfo.head());
	send_message(socket,(netcomm_send::HeadPacket*)qlogin.get());
	return true;
}

bool Userlogic::OnThirdLogin(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg,const int len){
	scoped_ptr<netcomm_recv::ThirdLogin> login(new netcomm_recv::ThirdLogin(netbase));
	scoped_ptr<base_logic::LBSInfos> lbs_info;
	usersvc_logic::UserInfo userinfo;
	std::string token;
	bool r = false;
	int64 uid = 0;
	int error_code = login->GetResult();
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	//第三方登陆不再需要地址转化
	//lbs_info.reset(base_logic::LogicUnit::GetGeocderAndAddress((netcomm_recv::HeadPacket*)login.get()));
	//if(r)
		//login.get()->Reset();
	userinfo.set_city(login->location());
	userinfo.set_imei(login->imei());
	userinfo.set_machine(login->machine());
	userinfo.set_type(login->source());
	userinfo.set_nickname(login->nickanme());
	userinfo.set_sex(login->sex());
	userinfo.set_birthday(login->birthday());
	userinfo.set_head(login->head());
	userinfo.set_session(login->session());
	userinfo.set_source(login->source());
	userinfo.set_birthday(login->birthday());
	userinfo.set_plt(login->plat());
	userinfo.set_channel(login->channel());
	//存储用户信息
	usersvc_logic::DBComm::OnThirdLogin(userinfo,lbs_info.get());
	//存入缓存
	data_engine_->SetUserInfo(userinfo.uid(),userinfo);
	base_logic::LogicUnit::CreateToken(userinfo.uid(),token);
	userinfo.set_token(token);
	scoped_ptr<netcomm_send::Login> qlogin(new netcomm_send::Login());
	uid = userinfo.uid();
	qlogin->set_userinfo(userinfo.Release());
	send_message(socket,(netcomm_send::HeadPacket*)qlogin.get());
	//启动通知机器人上线
	NoticeUserLogin(robot_server_socket_,10000,uid,0,0);
	return true;
}

bool Userlogic::OnLoginRecord(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg,const int len){
	scoped_ptr<netcomm_recv::LoginRecord> login(new netcomm_recv::LoginRecord(netbase));
	scoped_ptr<base_logic::LBSInfos> lbs_info;
	bool r = false;
	int error_code = login->GetResult();
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	lbs_info.reset(base_logic::LogicUnit::GetGeocderAndAddress((netcomm_recv::HeadPacket*)login.get()));

	//记录用户登陆时间和登陆位置及坐标
	usersvc_logic::DBComm::OnLoginRecord(login->uid(),login->plt(),login->login_machine(),
			lbs_info.get());

	int32 new_num = 0;
	//返回用户新增消息
	usersvc_logic::UserDicComm::GetNewMsgNum(login->uid(),new_num);

	scoped_ptr<netcomm_send::LoginRecord> qlogin(new netcomm_send::LoginRecord());
	qlogin->set_new_msg(2);
	send_message(socket,(netcomm_send::HeadPacket*)qlogin.get());

	//启动通知机器人上线
	NoticeUserLogin(robot_server_socket_,10000,login->uid(),0,0);
	return true;
}



bool Userlogic::OnUpdateUserInfo(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
		const void* msg,const int len){

	scoped_ptr<netcomm_recv::UpdateUserInfo> update(new netcomm_recv::UpdateUserInfo(netbase));
	int32 error_code = update->GetResult();
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}

	usersvc_logic::UserInfo userinfo;
	userinfo.set_uid(update->uid());
	userinfo.set_birthday(update->birthday());
	userinfo.set_nickname(update->nickname());
	userinfo.set_sex(update->sex());
	//更新信息
	usersvc_logic::DBComm::OnUpdateUserInfo(userinfo);

	scoped_ptr<netcomm_send::HeadPacket> qupdate(new netcomm_send::HeadPacket);
	qupdate->set_status(1);
	send_message(socket,(netcomm_send::HeadPacket*)qupdate.get());
	return true;
}



bool Userlogic::OnBDBindPush(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
   		const void* msg,const int len){
	scoped_ptr<netcomm_recv::BDBindPush> bind(new netcomm_recv::BDBindPush(netbase));
	/*
	 * const int64 platform,const int64 uid,const int64 channel,const int64 bd_userid,
			const std::string& pkg_name,const std::string& tag,const int64 appid,
			const int64 requestid,const int32 machine)
	 * */
	base_push::BaiduBindPushInfo bindinfo(bind->platform(),bind->uid(),bind->channel(),
			bind->bduserid(),bind->pkg_name(),bind->tag(),bind->appid(),bind->request(),
			bind->machine());
	base_push::PushConnectorEngine::GetPushConnectorEngine()->BindPushUserinfo(bindinfo);

	return true;
}


}

