#include "manager_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "basic/errno_comm.h"
#include "config/config.h"
#include "basic/radom_in.h"
#include "storage/dic_storage.h"
#include "json/json.h"
#include <sstream>

#define TIME_CALLBACK_USER    10025

namespace manager_logic{


ManagerEngine::ManagerEngine(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	manager_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	manager_storage::DBComm::Init(config->mysql_db_list_);
	manager_storage::MemComm::Init(config->mem_list_);
	manager_storage::RedisComm::Init(config->redis_list_);

}

ManagerEngine::~ManagerEngine(){
	manager_logic::ThreadKey::DeinitThreadKey ();
}

ManagerEngine* ManagerEngine::instance_ = NULL;
ManagerEngine* ManagerEngine::GetInstance(){
   if (instance_==NULL){
        instance_ = new ManagerEngine();
   }

   return instance_;
}

void ManagerEngine::FreeInstance(){
	delete instance_;
}

bool ManagerEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool ManagerEngine::OnBroadcastConnect(struct server *srv, int socket,
									  void *data, int len){
    return true;
}

bool ManagerEngine::OnBroadcastMessage(struct server *srv, int socket, void *msg, int len){
    return true;
}

bool ManagerEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
	srv->add_time_task((struct server*)srv,"manager",TIME_CALLBACK_USER,600,-1);
    return true;
}

bool ManagerEngine::OnTimeout(struct server *srv, char *id, int opcode, int time){
	switch(opcode){
	case TIME_CALLBACK_USER:
		OnCallBackUserInfo();
		break;
	default:
		LOG_ERROR2("unkown code :%d",opcode);
	}
    return true;
}

bool ManagerEngine::OnManagerClose(struct server *srv, int socket){
    return true;
}

bool ManagerEngine::OnManagerConnect(struct server *srv, int socket){
    return true;
}

bool ManagerEngine::OnManagerMessage(struct server *srv, int socket,
								   const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);

	if(type=="pushusermsg"){
		PushUserMsg(socket,packet);
	}else if(type=="pushusersmsg"){
		PushAllUserMsg(socket,packet);
	}
    return true;
}

/*
 *  http://api.url.com/pushusermsg.fcgi
 *   uid=10000
 *   isrecord = 1
 *   ispushmsg = 1
 *   message = 12321323
 * */

bool ManagerEngine::PushAllUserMsg(const int socket,const packet::HttpPacket& packet){

    bool r = false;
    packet::HttpPacket pack = packet;
    std::string str_tid;
    std::string message;
    std::string summary;
    std::string is_record;
    std::string is_push;
    std::string token;
    int64 tid = 10108;
    int64 platform_id;
    std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
    std::list<int64> userinfo_list;

    r = pack.GetAttrib("isrecord",is_record);
    if(!r){
    	is_record = "0";
    }

    r = pack.GetAttrib("ispush",is_push);
    if(!r){
    	 is_push = "1";
    }

    //消息
    r = manager_storage::DBComm::GetPushMessage(summary,message);
    if(!r)
    	return false;

    r = manager_storage::DBComm::GetUsersInfo(0,300,userinfo_list);

    while(userinfo_list.size()>0){
    	int64 uid = userinfo_list.front();
    	userinfo_list.pop_front();
    	if(uid==10108||uid ==10149||uid==10150||uid==10203)
    		PushUserMessage(uid,summary,message,atol(is_record.c_str()),atol(is_push.c_str()));
    }

ret:
	reponse = wr.write(result);
	r =  manager_logic::SomeUtils::SendFull(socket, reponse.c_str(), reponse.length());
    return r;

	return true;
}
/*
 *  http://api.url.com/pushusermsg.fcgi
 *   uid=10000
 *   isrecord = 1
 *   ispushmsg = 1
 *   message = 12321323
 * */

bool ManagerEngine::PushUserMsg(const int socket,const packet::HttpPacket& packet){

    bool r = false;
    packet::HttpPacket pack = packet;
    std::string str_tid;
    std::string summary;
    std::string message;
    std::string is_record;
    std::string is_push;
    std::string token;
    int64 tid;
    int64 platform_id;
    std::string reponse;
    Json::Value result;
    Json::FastWriter wr;



    r = pack.GetAttrib("uid",str_tid);
    if((!r)||(atoll(str_tid.c_str())<=0)){
   	 result["status"] = "1";
   	 result["msg"] = "用户ID不存在";
   	 result["result"]["chat"];
   	 goto ret;
    }

    tid = atoll(str_tid.c_str());


    r = pack.GetAttrib("message",message);
    if(!r){
       	 result["status"] = "1";
       	 result["msg"] = "消息不存在";
       	 result["result"]["chat"];
       	 goto ret;
    }


    r = pack.GetAttrib("isrecord",is_record);
    if(!r){
    	is_record = "0";
    }

    r = pack.GetAttrib("ispush",is_push);
    if(!r){
    	 is_push = "1";
    }

    PushUserMessage(tid,summary,message,atol(is_record.c_str()),atol(is_push.c_str()));

ret:
	reponse = wr.write(result);
	r =  manager_logic::SomeUtils::SendFull(socket, reponse.c_str(), reponse.length());
    return r;

	return true;
}

void ManagerEngine::OnCallBackUserInfo(){

	std::list<int64> userinfo_list;
	bool r = false;
	r = manager_storage::DBComm::GetOverTimeNoLogin(0,1000,userinfo_list);
	std::string summary="快回来吧...";
	std::string message = "快回来吧...";
	while(userinfo_list.size()>0){
		int64 uid = userinfo_list.front();
	    userinfo_list.pop_front();
	    if(uid==10108||uid ==10161||uid==10150||uid==10149)
	    	PushUserMessage(uid,summary,message,0,1);
	}

}

void ManagerEngine::PushUserMessage(const int64 recvid,const std::string& summary,
		const std::string& message,const int32 is_record,
		const int32 is_push){

	int64 msg_id;
	time_t current_time;

	if(is_record){
		manager_storage::RedisComm::GenaratePushMsgID(msg_id);
		current_time = time(NULL);
		OffLineMessage(1000,msg_id,current_time,recvid,summary);
		LeaveMessage(10000,msg_id,current_time,recvid, message);
	}
	if(is_push==1)
		PushMessage(10000,recvid,summary);
}


bool ManagerEngine::PushMessage(const int64 platform_id,const int64 recvid,
		const std::string& message){
	std::stringstream os;
	std::string device_token;
	bool is_receive;
	unsigned begin_time;
	unsigned end_time;
	os<<"助理小哟提醒"<<":"<<message;
	//get decivce token and  begin_time/end_time
	manager_storage::RedisComm::GetUserPushConfig(recvid,
			device_token,is_receive,begin_time,end_time);
	LOG_DEBUG2("device_token:%s", device_token.c_str());
	if(device_token.empty()||device_token=="(null)")
		return false;
	return manager_logic::HttpComm::PushMessage(device_token,os.str());
}


bool ManagerEngine::OffLineMessage(const int64 platform_id,const int64 msg_id,
		                           const time_t current_time,
		                           const int64&  recv_uid,
		                           const std::string& message){

	std::stringstream  s_send_uid;
	std::stringstream s_recv_uid;
	std::string detail;
	std::string summary;
	std::string s_current_time;

	int64 uid = 10000;
	manager_logic::SomeUtils::GetCurrentTimeFormat(current_time,s_current_time);

	s_send_uid<<uid;
	s_recv_uid<<recv_uid;

	//缓存redis部分
	manager_logic::SomeUtils::MakeLeaveContent(s_send_uid.str(),s_recv_uid.str(),msg_id,
												message,detail,summary,s_current_time);

	manager_storage::RedisComm::StagePushMsg(recv_uid,msg_id,detail);

	return true;
}

bool ManagerEngine::LeaveMessage(const int64 platform_id,const int64 msg_id,const time_t current_time,
		const int64& recvid,const std::string& message){

	std::string s_current_time;
	int64 uid = 10000;
	manager_logic::SomeUtils::GetCurrentTimeFormat(current_time,s_current_time);
	return manager_storage::DBComm::RecordMessage(platform_id,uid,recvid,
												msg_id,message,s_current_time);
}


}
