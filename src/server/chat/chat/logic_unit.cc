#include "logic_unit.h"
#include "dic_comm.h"
#include "db_comm.h"
#include "http_response.h"
#include "base/logic_comm.h"
#include "json/json.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
#include "log/mig_log.h"
#include <sstream>
#include <sys/socket.h>
#include <time.h>

static const std::string URL_PUSH_SERVICE("121.199.32.88/api/push");

namespace chat_logic{

bool LogicUnit::CheckToken(const int64 platform_id,int64 user_id,
						   const char* token){

	return chat_storage::MemComm::CheckToken(platform_id,user_id,token);
}

bool LogicUnit::GetUserInfo(const int64 platform_id,int64 user_id,chat_base::UserInfo& userinfo){
	bool r = false;
	r = chat_storage::MemComm::GetUserInfo(platform_id,user_id,userinfo);
	if (r)
		return r;
	r = chat_storage::DBComm::GetUserInfo(platform_id,user_id,userinfo);
	return r;
	
}

bool LogicUnit::SetChatToken(chat_base::UserInfo& userinfo){
	//create token
	std::string token;
	int32 random_num = base::SysRadom::GetInstance()->GetRandomID();
	//md5
	token="miglab";
	std::stringstream os;
	std::string key;
	os<<random_num;
	MD5Sum md5(os.str());
	token = md5.GetHash();
	userinfo.set_chat_token(token);
	return true;
}

bool LogicUnit::CheckChatToken(const chat_base::UserInfo& userinfo,
		const char* token){
    if(strcmp(userinfo.chat_token().c_str(),token)==0)
    	return true;
    return false;
}

bool HttpComm::PushMessage(const std::string &device_token,
		const std::string &msg, int badge/*=1*/, const std::string &sound/*=""*/) {

	//return true;
	if (device_token.empty())
		return false;

	chat_logic::HttpPost post(URL_PUSH_SERVICE);
	Json::Value value;
	Json::FastWriter wr;
	std::string post_str;
	value["message"] = msg;
	value["badge"] = badge;
	value["devicetoken"] = device_token;
	value["sound"] = sound;
	post_str = wr.write(value);

	LOG_DEBUG2("Push msg post:%s", post_str.c_str());
	int port = 9090;
	try {
		post.Post(post_str.c_str(),port);
		std::string result;
		post.GetContent(result);
		LOG_DEBUG2("%s",result.c_str());

		Json::Reader rd;
		Json::Value value;
		if (!rd.parse(result, value)) {
			LOG_DEBUG("Push msg failed: ÍÆËÍÊ§°Ü");
			return false;
		}

	} catch (const std::exception &ex) {
		LOG_DEBUG2("Push msg failed:%s", ex.what());
		return false;
	}catch (...) {
		LOG_DEBUG("Push msg failed");
		return false;
	}
	return true;
}

} 
