#include "push_baidu_connector.h"
#include "push_logic_unit.h"
#include "push_db_comm.h"
#include "logic/logic_comm.h"
#include "http/http_method.h"
#include "json/json.h"
#include "basic/md5sum.h"
#include "basic/basic_util.h"
#include "file/file_path.h"
#include "file/file_util.h"
#include <sstream>
namespace base_push{

PushBaiduConnectorImpl::PushBaiduConnectorImpl()
:host_("http://channel.api.duapp.com/")
,version_("2.0")
,channel_("channel"){
}

PushBaiduConnectorImpl::~PushBaiduConnectorImpl(){

}

void PushBaiduConnectorImpl::Init(std::list<base::ConnAddr>& addrlist){
	key_ = "Yw3DNoCNaW3PtOzKXsOolp4c";
	secret_ = "z1OPnsDsDGIVocA4XF5qy268o66vFmGG";
	base_push::PushDBComm::Init(addrlist);
}

void PushBaiduConnectorImpl::Release(){

}


void PushBaiduConnectorImpl::PushUserMessage(const int64 platform,const int64 uid,
		const std::string& title,const std::string& description,
		const int32 level,
		const int32 badge){
	base_push::BaiduPushMessage push_message(platform,uid,level);
	std::string message;
	std::stringstream os;
	std::string url;
	std::string host = "http://www.baidu.com";
	os<<host_<<"rest/"<<version_<<"/channel/"<<channel_;
	url = os.str();
	base_push::PushDBComm::GetUserBdInfo(platform,uid,push_message);
	push_message.set_title(title);
	push_message.set_description(description);
	push_message.set_url(host);
	push_message.set_ios_cunstom_content("badge",badge);
	PushUserMessage(url,1,push_message);
}

void PushBaiduConnectorImpl::PushAllUserMessage(const std::string& title,
		const std::string& description,
		const int32 level,
		const int32 badge){

}


void PushBaiduConnectorImpl::TimeDBPushMessage(){
	std::stringstream os;
	std::string url;
	bool r = false;
	os<<host_<<"rest/"<<version_<<"/channel/"<<channel_;
	url = os.str();
	std::list<base_push::BaiduPushMessage*> list;
	base_push::PushDBComm::GetAllPushMessage(list);
	while(list.size()>0){
		base_push::BaiduPushMessage* push_message = list.front();
		list.pop_front();
		//android
		push_message->set_machine(ANDROID_TYPE);
		r = PushUserMessage(url,3,(*push_message));
		//IOS
		push_message->set_machine(IOS_TYPE);
		r = PushUserMessage(url,3,(*push_message));
		if(r)
			base_push::PushDBComm::UpdateAllPushMessage(push_message->msg_id());
		if(push_message){
			delete push_message;
			push_message = NULL;
		}
	}
}

bool PushBaiduConnectorImpl::BindPushUserinfo(base_push::BaiduBindPushInfo& push_info){
	//写入数据库
	/**
	 * const int64 platform,const int64 uid,const int64 channel,
			const std::string& baidu_userid,const std::string& pkg_name,const std::string& tag,
			const int64 appid,const int64 requestid,const int32 machine)
	 */
	bool r = base_push::PushDBComm::BindBDPushUserinfo(push_info.platform(),push_info.uid(),
			push_info.channel(),push_info.bd_userid(),push_info.pkg_name(),
			push_info.tag(),push_info.appid(),push_info.requestid(),push_info.machine());
	return true;
}

bool PushBaiduConnectorImpl::BindPushUserinfo(packet::HttpPacket& packet,int& err,
		std::string& err_str){
	//解析信息
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string platform_str;
	std::string uid_str;
	std::string channel_str;
	std::string userid_str;
	std::string pkg_name_str;
	std::string tag_str;
	std::string machine_str;
	std::string appid_str;
	std::string request_str;
	r = base_push::PushNet::OnBDBindPush(pack,platform_str,uid_str,channel_str,
			userid_str,pkg_name_str,tag_str,machine_str,appid_str,request_str,err);
	if(!r){
		err_str = base_push::push_http_strerror(err);
		return false;
	}
	//写入数据库
	r = base_push::PushDBComm::BindBDPushUserinfo(atoll(platform_str.c_str()),atoll(uid_str.c_str()),
			atoll(channel_str.c_str()),userid_str,pkg_name_str,tag_str,appid_str,request_str,
			atoll(machine_str.c_str()));
	if(!r){
		err = PUSH_DB_STORAGE;
		err_str = base_push::push_http_strerror(err);
		return false;
	}

	return true;
}


bool PushBaiduConnectorImpl::PushUserMessage(const std::string& url,const int32 push_type,
		base_push::BaiduPushMessage& push_message){
	std::string message;
	std::string content;
	bool r = false;
	r = push_message.SerializedJson(message);
	if(!r)
		return false;

	LOG_DEBUG2("message %s",message.c_str());
	base_push::BaiDuPushMsgInfo push_msg;
	push_msg.set_method("POST");
	push_msg.set_url(url);
	push_msg.set_secret(secret_);
	push_msg.set_value("channel_id",push_message.channel());
	push_msg.set_value("message_type","1");
	push_msg.set_value("method","push_msg");
	push_msg.set_value("messages",message);
	//if(push_message.machine()==1)
	push_msg.set_value("msg_keys",time(NULL)+11);
	push_msg.set_value("push_type",push_type);
	if(push_message.machine()==ANDROID_TYPE)
		push_msg.set_value("device_type",base_push::ANDRIOD);
	else if(push_message.machine()==IOS_TYPE)
		push_msg.set_value("device_type",base_push::IOS);

	if(push_message.machine()==IOS_TYPE)
		push_msg.set_value("deploy_status",DEPLOY_STATUS);

	push_msg.set_value("timestamp",time(NULL));
	push_msg.set_value("expires",time(NULL)+100);
	push_msg.set_value("user_id",push_message.baidu_user_id());
	push_msg.set_value("apikey",key_);
	push_msg.GetContent(content);
	if(!content.empty())
		r = PushBaiduMessage(url,content);
	return r;
}

bool PushBaiduConnectorImpl::PushUserMessage(const std::string& url,
		const std::string& userid,const int64 channel,
		const std::string& title,const std::string& description){
	std::string message;
	std::string content;
	bool r = false;
	MakePushMessage(title,description,message);
	base_push::BaiDuPushMsgInfo push_msg;
	push_msg.set_method("POST");
	push_msg.set_url(url);
	push_msg.set_secret(secret_);
	push_msg.set_value("channel_id",channel);
	push_msg.set_value("message_type","1");
	push_msg.set_value("method","push_msg");
	push_msg.set_value("messages",message);
	//push_msg.set_value("msg_keys",time(NULL)+11);
	push_msg.set_value("expires",time(NULL)+1000);
	push_msg.set_value("push_type","1");
	push_msg.set_value("timestamp",time(NULL));
	push_msg.set_value("user_id",userid);
	push_msg.set_value("apikey",key_);
	push_msg.GetContent(content);
	if(!content.empty())
		r = PushBaiduMessage(url,content);
	return r;
}



bool PushBaiduConnectorImpl::PushBaiduMessage(const std::string& url,
		const std::string& content){
	std::string repsones;
	int64 request_id;
	int32 success_amount;
	std::string resouce_ids;
	int32 eror_code;
	std::string err_msg;
	bool r = false;
	r = PushBaiduMsg(url,content,repsones);
	if(!r)
		return r;
	//解析
	r = base_push::ResolveJson::ReolveJsonBaiduPushMsg(repsones,request_id,
			success_amount,resouce_ids,eror_code,err_msg);
	if(r)
		LOG_MSG2("request_id %lld,success_amount %lld resouce_ids %s",
			request_id,success_amount,resouce_ids.c_str());
	else
		LOG_ERROR2("eror_code %d err_msg %s",eror_code,err_msg.c_str());
	return true;
}

bool PushBaiduConnectorImpl::PushBaiduMsg(const std::string& url,const std::string& content,
		std::string& reponse){
	//发送
	http::HttpMethodPost http_post(url);
	bool r = http_post.Post(content.c_str(),0,false);
	r = http_post.GetContent(reponse);

	LOG_DEBUG2("%s",reponse.c_str());
	return r;
}


void PushBaiduConnectorImpl::MakePushMessage(const std::string& title,const std::string& description,std::string& message){
	Json::Value result;
	Json::FastWriter wr;
	std::string content;
	result["title"] = title;
	result["description"] = description;
	result["url"] = "http://www.baidu.com";
	//result["pkg_name"] = "com.workmall.bluecollar";
	result["open_type"] = 1;
	content = wr.write(result);
	message = content.substr(0,content.length() -1);
}





void PushBaiduConnectorImpl::TestSign(){
	bool r =false;
	std::stringstream os;
	std::stringstream os1;
	std::stringstream ostime;
	const time_t current = time(NULL);
	ostime<<current;
	std::string channel_id = "4209320236570070120";
	//std::string message = "{'title':'2222','description':'11111'}";
	std::map<std::string,std::string> map_sign;
	std::string message;
	std::string title = "哈哈";
	std::string description = "哈哈";
	std::string msg_keys = "5785785785dsasdasdasd";
	std::string user_id = "614042184531593323";
	std::string url = "http://channel.api.duapp.com/rest/2.0/channel/channel";
	MakePushMessage(title,description,message);

	map_sign["channel_id"] = channel_id;
	map_sign["message_type"] = "1";
	map_sign["messages"] = message;
	map_sign["method"] = "push_msg";
	map_sign["msg_keys"] = msg_keys;
	map_sign["push_type"] = "1";
	map_sign["timestamp"] = ostime.str();
	map_sign["user_id"] = user_id;
	map_sign["apikey"] = "Yw3DNoCNaW3PtOzKXsOolp4c";
	std::stringstream os3;

	os3<<"POSThttp://channel.api.duapp.com/rest/2.0/channel/channel";
	 for (std::map<std::string, std::string>::iterator iter = map_sign.begin();
	       iter != map_sign.end();
	       ++iter) {
		 os3<<iter->first<<"="<<iter->second;
	  }
	 LOG_DEBUG2("%s",os3.str().c_str());

	/*os<<"POSThttp://channel.api.duapp.com/rest/2.0/channel/channelapikey=Yw3DNoCNaW3PtOzKXsOolp4cchannel_id="
			<<4209320236570070120<<"message_type=1messages="<<message<<"method=push_msgmsg_keys="
			<<msg_keys<<"push_type=1timestamp="
			<<current<<"user_id="<<user_id<<"z1OPnsDsDGIVocA4XF5qy268o66vFmGG";*/
	os3<<"z1OPnsDsDGIVocA4XF5qy268o66vFmGG";
	std::string content = os3.str();
	//std::string content = "POSThttp://channel.api.duapp.com/rest/2.0/channel/channelapikey=Yw3DNoCNaW3PtOzKXsOolp4cchannel_id=4209320236570070120message_type=1messages={'title':'2222','description':'11111'}method=push_msgmsg_keys=5785785785dsasdasdasdpush_type=1timestamp=1411716673user_id=614042184531593323z1OPnsDsDGIVocA4XF5qy268o66vFmGG";
	std::string url_content;
	std::string hexdigest;
	base::BasicUtil::UrlEncode(content,url_content);
	base::MD5Sum md5sum(url_content);
	hexdigest = md5sum.GetHash();
	LOG_DEBUG2("%s",content.c_str());
	LOG_DEBUG2("%s",url_content.c_str());
	LOG_DEBUG2("%s",hexdigest.c_str());

	//测试
	base_push::BaiDuPushMsgInfo msgsign;
	msgsign.set_method("POST");
	msgsign.set_url("http://channel.api.duapp.com/rest/2.0/channel/channel");
	msgsign.set_secret("z1OPnsDsDGIVocA4XF5qy268o66vFmGG");
	msgsign.set_value("channel_id",channel_id);
	msgsign.set_value("message_type","1");
	msgsign.set_value("method","push_msg");
	msgsign.set_value("messages",message);
	msgsign.set_value("msg_keys",msg_keys);
	msgsign.set_value("push_type","1");
	msgsign.set_value("timestamp",current);
	msgsign.set_value("user_id",user_id);
	msgsign.set_value("apikey","Yw3DNoCNaW3PtOzKXsOolp4c");
	std::string getsign;
	msgsign.GetSign(getsign);
	LOG_DEBUG2("======%s=======",getsign.c_str());

	//发送
	http::HttpMethodPost http_post(url);
	os1<<"apikey=Yw3DNoCNaW3PtOzKXsOolp4c"
		<<"&user_id="<<user_id
		<<"&timestamp="<<current
		<<"&push_type=1"
		<<"&messages="<<message
		<<"&sign="<<hexdigest
		<<"&channel_id="<<channel_id
		<<"&msg_keys="<<msg_keys
		<<"&message_type=1&method=push_msg";
	std::string request_content = os1.str();
	std::string reply_content;

	r = http_post.Post(request_content.c_str(),0,false);

	r = http_post.GetContent(reply_content);

	LOG_DEBUG2("%s",reply_content.c_str());
}




}
