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
		return true;
	r = chat_storage::DBComm::GetUserInfo(platform_id,user_id,userinfo);
	/*if(type==1)
		r = chat_storage::DBComm::GetUserInfo(platform_id,user_id,userinfo);
	else if(type==2)
		r = chat_storage::DBComm::GetRobotsUserInfo(platform_id,user_id,userinfo);
	*/
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


bool LogicUnit::MakeLeaveContent(const chat_base::UserInfo& send_userinfo,
        const chat_base::UserInfo& recv_userinfo,const std::string& msg,
        const int msg_type,std::string &summary,double& distance){

	bool r = false;
	summary.clear();
	double uid_latitude,uid_longitude,tar_latitude,tar_longitude;

	r = chat_storage::DBComm::GetUserLBSPos(send_userinfo.user_id(),
			uid_latitude,uid_longitude);
	if(!r){
		uid_latitude = uid_longitude = 0;
	}

	r = chat_storage::DBComm::GetUserLBSPos(recv_userinfo.user_id(),
			tar_latitude,tar_longitude);
	if(!r){
		tar_latitude = tar_longitude = 0;
	}

	SummaryContent::OnSummaryContent(send_userinfo,recv_userinfo,msg,msg_type,summary);
	distance = base::BasicUtil::CalcGEODistance(uid_latitude,uid_longitude,tar_latitude,tar_longitude);
	return true;
}




bool LogicUnit::MakeLeaveContent(const std::string& send_uid,const std::string& to_uid,
		int64 msg_id,const std::string& msg,std::string& detail,std::string &summary
		,std::string& current){
//注释- 留言消息从redis中修改到数据库 暂不需要json 格式化
/*	std::stringstream ss;
	char tmp[256] = {0};
	Json::FastWriter wr;
	Json::Value value;
	value["action"] = "leavemsg";
	snprintf(tmp, arraysize(tmp), "%lld", msg_id);
	value["msgid"] = tmp;
	Json::Value &content = value["content"];
	content["send_uid"] = send_uid.c_str();
	content["to_uid"] = to_uid.c_str();
	content["msg"] = msg;
	content["time"] = current;
	detail = wr.write(value);
*/
	return true;
}

void LogicUnit::GetCurrentTimeFormat(const time_t current,std::string& current_time)
{
	struct tm* local = localtime(&current);
	std::stringstream os;
	os<<(1900+local->tm_year)<<"-"
	  <<(1+local->tm_mon)<<"-"
	  <<local->tm_mday<<" "
	  <<local->tm_hour<<":"
	  <<local->tm_min<<":"
	  <<local->tm_sec;
	current_time = os.str();
}

void LogicUnit::GetCurrntTimeFormat(std::string& current_time){
	time_t current = time(NULL);
	GetCurrentTimeFormat(current,current_time);
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
			LOG_DEBUG("Push msg failed: ����ʧ��");
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


bool SummaryContent::OnSummaryContent(const chat_base::UserInfo& send_userinfo,
        const chat_base::UserInfo& recv_userinfo,
        const std::string& msg,const int msg_type,
        std::string & summary){

	switch(msg_type){
	case TYPE_TEXT:
		SummaryTestContent(send_userinfo,recv_userinfo,msg,summary);
		break;
	default:
		break;
	}
	return true;
}

//文本简略
bool SummaryContent::SummaryTestContent(const chat_base::UserInfo& send_userinfo,
        const chat_base::UserInfo& recv_userinfo,const std::string& msg,
        std::string & summary){

	std::string content;
	int32 flag = 0;
	if(msg.length()<=MAX_SUMMARY_CONTENT_LEN){
		flag = 0;
		content = msg;
	}else{
		flag = 1;
		content = msg.substr(0,MAX_SUMMARY_CONTENT_LEN);
		//从最后一位开始检查是否为表情符号
		//先查找"["

		do{
			size_t pos = content.rfind("[");
			if(pos==std::string::npos)//不存在表情符号
				break;
			std::string rstr;
			rstr = content.substr(pos,content.length());
			size_t rpos = rstr.find("]");
			if(rpos!=std::string::npos)//存在，是一个完整的表情符号
				break;
			//截取掉表情前一部分
			content = content.substr(0,pos);
		}while(0);
	}

	if(flag){
		summary = content+"......";
	}else{
		summary = content;
	}
	return true;
}

} 
