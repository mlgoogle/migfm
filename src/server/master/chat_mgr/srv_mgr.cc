#include "srv_mgr.h"
#include "db_comm.h"
#include "logic_comm.h"
#include "json/json.h"
#include <list>
#include <string>

namespace chat_logic{

ServerMgr::ServerMgr(){

}

ServerMgr::~ServerMgr(){

}

bool ServerMgr::GetBestIdle(const int socket,const packet::HttpPacket& packet){
    bool r = false;
    packet::HttpPacket pack = packet;
    std::string str_uid;
    std::string str_tid;
    std::string str_platform_id;
    std::string token;
    int64 uid;
    int64 tid;
    int64 platform_id;
    std::string reponse;
    Json::Value result;
    Json::FastWriter wr;


    r = pack.GetAttrib("platformid",str_platform_id);
    if((!r)||(atoll(str_platform_id.c_str())<=0)){
   	 result["status"] = "1";
   	 result["msg"] = "平台ID不存在";
   	 result["result"]["chat"];
   	 goto ret;
    }
    platform_id = atoll(str_platform_id.c_str());


  	 result["status"] = 1;
  	 result["msg"] = "";
  	 result["result"]["host"] = "42.121.14.108";
  	 result["result"]["port"] = 17000;

ret:
	reponse = wr.write(result);
	r =  chat_logic::SomeUtils::SendFull(socket, reponse.c_str(), reponse.length());
    return r;

	return r;
}
}
