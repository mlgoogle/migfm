#include "user_mgr.h"
#include "db_comm.h"
#include "dic_comm.h"
#include <list>
#include <string>

namespace robot_logic{

UserMgr::UserMgr(){

}

UserMgr::~UserMgr(){

}

//getmails.fcgi
// token ="31231231231232132"
// count = 10
// from = 0;
bool UserMgr::GetUserInfoMail(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
    std::string token;
    std::string str_count;
    std::string str_from;
    int64 count = 0;
    int64 from = 0;
    Json::Value& content = result["result"];
    std::list<robot_base::MailUserInfo> user_list;

    /*if (!pack.GetAttrib("token", token)) {
	    result["status"] = 0;
	    result["msg"] = "无验证码";
	    goto ret;
    }*/

    if (!pack.GetAttrib("count", str_count)) {
    	str_count = "10";
    }
    if (!pack.GetAttrib("from", str_from)) {
    	str_from = "0";
    }
    count = atoll(str_count.c_str());
    from = atoll(str_from.c_str());

    if(count<0)
    	count = 0;
    if(from<0)
    	from = 0;

    r = robot_storage::DBComm::GetMailUserInfo(from,count,user_list);
    if(!r){
	    result["status"] = 0;
	    result["msg"] = "无用户";
	    goto ret;
    }
    GetUserInfoMail(user_list,content);
    result["status"] = 1;
    result["msg"] = "";

ret:
	reponse = wr.write(result);
	r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
       		 reponse.length());
}

void UserMgr::GetUserInfoMail(std::list<robot_base::MailUserInfo>& list, Json::Value& value){
	while(list.size()>0){
		robot_base::MailUserInfo mailinfo = list.front();
		list.pop_front();
		Json::Value  info;
		if(CheckUserInfo(mailinfo.username())){
			info["id"] = (Json::Int64)(mailinfo.uid());
			info["name"] = mailinfo.username().c_str();
			info["nickname"] = mailinfo.nickname().c_str();
			value.append(info);
		}
	}
}

bool UserMgr::CheckUserInfo(const std::string& username){
	return true;
}
}
