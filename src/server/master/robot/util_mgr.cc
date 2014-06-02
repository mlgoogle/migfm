#include "util_mgr.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "basic/base64.h"
#include <list>
#include <string>

namespace robot_logic{

UtilMgr::UtilMgr(){

}

UtilMgr::~UtilMgr(){

}

bool UtilMgr::GetSpreadMail(const int socket,const packet::HttpPacket& packet){
//getmails.fcgi
// token ="31231231231232132"
// count = 10
// from = 0;
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
    std::string title;
    std::string mailcontent;
    std::string b64title;
    std::string b64mailcontent;
    std::string token;
    int64 count = 0;
    int64 from = 0;
    Json::Value& content = result["result"];

    /*if (!pack.GetAttrib("token", token)) {
	    result["status"] = 0;
	    result["msg"] = "无验证码";
	    goto ret;
    }*/

    r = robot_storage::DBComm::GetSpreadMail(title,mailcontent);
    if(!r){
	    result["status"] = 0;
	    result["msg"] = "无内容";
	    goto ret;
    }

    //使用base64 编码， 避免邮件内容中特殊符号导致的问题
    Base64Encode(title,&b64title);
    Base64Encode(mailcontent,&b64mailcontent);
    content["title"] = b64title;
    content["content"] = b64mailcontent;

    result["status"] = 1;
    result["msg"] = "";

ret:
	reponse = wr.write(result);
	r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
       		 reponse.length());
}


}
