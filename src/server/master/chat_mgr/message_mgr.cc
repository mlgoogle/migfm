#include "message_mgr.h"
#include "db_comm.h"
#include "json/json.h"
#include <list>
#include <string>

namespace chat_logic{

MessageMgr::MessageMgr(){

}

MessageMgr::~MessageMgr(){

}

bool MessageMgr::GetLeaveMessage(const int socket,const packet::HttpPacket& packet){
     bool r = false;
     packet::HttpPacket pack = packet;
     std::list<struct GetLeaveMessage*> list;
     std::string str_platform_id;
     std::string str_uid;
     std::string str_oppid;
     std::string str_from;
     std::string str_count;
     std::string str_msg_id;
     std::string token;
     std::string reponse;
     std::string utf8_reponse;
     int64 platform_id = 0;
     int64 uid = 0;
     int64 oppid = 0;
     int32 from = 0;
     int32 count = 0;
     int64 msg_id = 0;
     int32 utf8_flag = 0;
     Json::Value result;
     Json::FastWriter wr;
     Json::Value& content =  result["result"]["chat"];

     r = pack.GetAttrib("platformid",str_platform_id);
     if((!r)||(atoll(str_platform_id.c_str())<=0)){
    	 result["status"] = "1";
    	 result["msg"] = "平台ID不存在";
    	 content = "";
    	 goto ret;
     }
     platform_id = atoll(str_platform_id.c_str());

     r = pack.GetAttrib("uid",str_uid);
     if((!r)||(atoll(str_uid.c_str())<=0)){
    	 result["status"] = "1";
    	 result["msg"] = "用户ID不存在";
    	 content = "";
    	 goto ret;
     }
     uid = atoll(str_uid.c_str());


     r = pack.GetAttrib("tid",str_oppid);
     if((!r)||(atoll(str_oppid.c_str())<=0)){
    	 result["status"] = "1";
    	 result["msg"] = "对方ID不存在";
    	 content = "";
    	 goto ret;
     }
     oppid = atoll(str_oppid.c_str());

     r = pack.GetAttrib("msgid",str_msg_id);
     if((!r)||(atoll(str_msg_id.c_str())<0)){
    	 result["status"] = "1";
    	 result["msg"] = "消息id不存在";
    	 content = "";
    	 goto ret;
     }
     msg_id = atoll(str_msg_id.c_str());

     r = pack.GetAttrib("fromid",str_from);
     if((!r)||(atoll(str_from.c_str())<=0)){
    	 from = 0;
     }else{
    	 from = atol(str_oppid.c_str());
     }


     r = pack.GetAttrib("count",str_count);
     if((!r)||(atoll(str_count.c_str())<=0)){
    	 count  = 5;
     }else{
    	 count = atol(str_count.c_str());
     }



     //token
     r = pack.GetAttrib("token",token);
     r = true;
     if(!r||(chat_logic::SomeUtils::CheckUserToken(str_platform_id,str_uid,token))){
    	 result["status"] = "1";
    	 result["msg"] = "token错误";
    	 content = "";
    	 goto ret;
     }

     r = chat_storage::DBComm::GetLeaveMessage(platform_id,uid,oppid,from,count,msg_id,list);
     if(!r){
    	 result["status"] = "0";
    	 result["msg"] = "";
    	 content = "";

     }
     result["status"] = "0";
     result["msg"] = "";

     PacketLeaveMessage(list,content);
ret:
     reponse = wr.write(result);
     //chat_logic::SomeUtils::GetUTF8(reponse,utf8_reponse,utf8_flag);
     r =  chat_logic::SomeUtils::SendFull(socket, reponse.c_str(),
    		 reponse.length());
     return r;
}

bool MessageMgr::PacketLeaveMessage(std::list<struct GetLeaveMessage*> list,Json::Value& chat_info){

    if(list.size()<=0)
    	return false;

    while(list.size()>0){
    	Json::Value content;
    	struct GetLeaveMessage* msg = list.back();
    	list.pop_back();
    	content["fid"] = msg->uid;
    	content["tid"] = msg->oppid;
    	content["id"] = msg->msg_id;
    	content["msg"] = msg->message;
    	content["time"] = msg->lasttime;
    	//GetLeaveMsgDetail(msg,content);
    	chat_info.append(content);
    	if(msg){
    		delete msg;
    		msg = NULL;
    	}
    }
    return true;
}

}
