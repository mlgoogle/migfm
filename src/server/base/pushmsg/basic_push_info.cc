#include "basic_push_info.h"
#include "basic/md5sum.h"
#include "basic/basic_util.h"
#include "log/mig_log.h"

namespace base_push{
BaiduBindPushInfo::BaiduBindPushInfo(){
	data_ = new Data();
}

BaiduBindPushInfo::BaiduBindPushInfo(const int64 platform,const int64 uid,const int64 channel,const int64 bd_userid,
		const std::string& pkg_name,const std::string& tag,const int64 appid,
		const int64 requestid,const int32 machine){
	data_ = new Data(platform,uid,channel,bd_userid,pkg_name,tag,
			appid,requestid,machine);
}


BaiduBindPushInfo::BaiduBindPushInfo(const BaiduBindPushInfo& bind)
:data_(bind.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

BaiduBindPushInfo& BaiduBindPushInfo::operator =(const BaiduBindPushInfo& bind){
	if (bind.data_!=NULL){
		bind.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = bind.data_;
	return *this;
}


BaiduPushCustomContent::BaiduPushCustomContent(){
	data_ = new Data();
}


BaiduPushCustomContent::BaiduPushCustomContent(const BaiduPushCustomContent& custom)
:data_(custom.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

BaiduPushCustomContent& BaiduPushCustomContent::operator =(const BaiduPushCustomContent& custom){
	if (custom.data_!=NULL){
		custom.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = custom.data_;
	return *this;
}

void BaiduPushCustomContent::set_value(const std::string& key,const std::string& value){
	data_->param_map_[key] = value;
}


void BaiduPushCustomContent::set_value(const std::string& key,const int64 value){
	std::stringstream os;
	os<<value;
	std::string s_value = os.str();
	set_value(key,s_value);
}

void BaiduPushCustomContent::set_value(const std::string& key,const int32 value){
	std::stringstream os;
	os<<value;
	std::string s_value = os.str();
	set_value(key,s_value);
}

void BaiduPushCustomContent::GetJsonObject(Json::Value& result){
	if(get_count()<=0)
		return;
	Json::Value& value = result[data_->name_];
	for(std::map<std::string,std::string>::iterator itr = data_->param_map_.begin();
			itr!=data_->param_map_.end();itr++){
		if(itr->first=="badge")
			value[itr->first] = atoll(itr->second.c_str());
		else
			value[itr->first] = itr->second;
	}
}

BaiduPushMessage::BaiduPushMessage(){
	data_ = new Data();
}

BaiduPushMessage::BaiduPushMessage(const int64 platform,const int64 uid){
	data_ = new Data(platform,uid);
}

BaiduPushMessage::BaiduPushMessage(const int64 platform,const int64 uid,const int32 level,
		const int64 msg_id){
	data_ = new Data(platform,uid,level,msg_id);
}

BaiduPushMessage::BaiduPushMessage(const BaiduPushMessage& push_message)
:data_(push_message.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

BaiduPushMessage& BaiduPushMessage::operator =(const BaiduPushMessage& push_message){
	if (push_message.data_!=NULL){
		push_message.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = push_message.data_;
	return *this;
}


void BaiduPushMessage::set_value(const std::string& key,const std::string& value){
	if(value.empty())
		return;
	data_->param_map_[key] = value;
}


void BaiduPushMessage::set_value(const std::string& key,const int64 value){
	if(value==0)
		return;
	std::stringstream os;
	os<<value;
	std::string s_value = os.str();
	set_value(key,s_value);
}

void BaiduPushMessage::set_value(const std::string& key,const int32 value){
	if(value==0)
		return;
	std::stringstream os;
	os<<value;
	std::string s_value = os.str();
	set_value(key,s_value);
}


bool BaiduPushMessage::SerializedJson(std::string& content){
	Json::Value result;
	std::string message;
	Json::Value android_value;
	Json::Value ios_value;
	Json::FastWriter wr;
	//内容不能为空
	if(data_->description_.empty())
		return false;
	//宏定义
#define JSON_VALUE_STR(key,value) \
	if(!value.empty())\
		result[key] = value;\


#define JSON_VALUE_INT(key,value) \
		result[key] = value;\
//判断安卓
	if(data_->machine_==ANDROID_TYPE){
		JSON_VALUE_STR("title",data_->title_)
		JSON_VALUE_STR("description",data_->description_)
		JSON_VALUE_INT("notification_builder_id",data_->notification_builder_id_)
		JSON_VALUE_INT("notification_basic_style",data_->notification_basic_style_)
		JSON_VALUE_INT("open_type",data_->open_type_)
		JSON_VALUE_INT("net_support",data_->net_support_)
		JSON_VALUE_INT("user_confirm",data_->user_confirm_)
		JSON_VALUE_STR("url",data_->url_)
		JSON_VALUE_STR("pkg_content",data_->pkg_content_)
		JSON_VALUE_STR("pkg_name",data_->pkg_name_);
	}
#undef JSON_VALUE_STR
#undef JSON_VALUE_INT

	//data_->android_cunstom_content_.GetJsonObject(result);
	if(data_->machine_==IOS_TYPE)
		data_->ios_cunstom_content_.GetJsonObject(result);
	GetJsonObject(result);

	message = wr.write(result);
	content = message.substr(0,message.length()-1);
	return true;
}

void BaiduPushMessage::GetJsonObject(Json::Value& result){
	if(data_->param_map_.size()<=0)
		return;
	for(std::map<std::string,std::string>::iterator itr = data_->param_map_.begin();
			itr!=data_->param_map_.end();itr++){
		result[itr->first] = itr->second;
	}
}

BaiDuPushMsgInfo::BaiDuPushMsgInfo()
:sign_change_(false)
,data_change_(false){
	data_ = new Data();
}


BaiDuPushMsgInfo::BaiDuPushMsgInfo(const BaiDuPushMsgInfo& push_msg_info)
:data_(push_msg_info.data_)
,sign_change_(false)
,data_change_(false){
	if(data_!=NULL){
		data_->AddRef();
	}
}

BaiDuPushMsgInfo& BaiDuPushMsgInfo::operator =(const BaiDuPushMsgInfo& push_msg_info){

	if (push_msg_info.data_!=NULL){
		push_msg_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = push_msg_info.data_;
	return *this;
}

void BaiDuPushMsgInfo::set_value(const std::string& key,const std::string& value){
	if(value.empty())
		return;
	data_->param_map_[key] = value;
	sign_change_ = true;
	data_change_ = true;
}

void BaiDuPushMsgInfo::set_value(const std::string& key,const int64 value){
	if(value==0)
		return;
	std::stringstream os;
	os<<value;
	std::string s_value = os.str();
	set_value(key,s_value);
}


void BaiDuPushMsgInfo::GetSign(std::string& hexdigest){
	if(hexdigest_.empty()||sign_change_)
		Calculate();
	hexdigest = hexdigest_;
}

void BaiDuPushMsgInfo::GetContent(std::string& content){
	if(content_.empty()||data_change_)
		Package();
	content = content_ ;
}

void BaiDuPushMsgInfo::Calculate(){
	std::stringstream os;
	std::string url_content;
	std::string content;
	os<<data_->method_;
	os<<data_->url_;
	for (std::map<std::string, std::string>::iterator iter = data_->param_map_.begin();
	       iter != data_->param_map_.end();
	       ++iter) {
		os<<iter->first<<"="<<iter->second;
	}
	os<<data_->secret_;
	content = os.str();
	base::BasicUtil::UrlEncode(content,url_content);

	base::MD5Sum md5sum(url_content);
	hexdigest_ = md5sum.GetHash();
	sign_change_ = false;
	/*os<<"POSThttp://channel.api.duapp.com/rest/2.0/channel/channelapikey=Yw3DNoCNaW3PtOzKXsOolp4cchannel_id="
			<<4209320236570070120<<"message_type=1messages="<<message<<"method=push_msgmsg_keys="
			<<msg_keys<<"push_type=1timestamp="
			<<current<<"user_id="<<user_id<<"z1OPnsDsDGIVocA4XF5qy268o66vFmGG";*/
}

void BaiDuPushMsgInfo::Package(){
	std::stringstream os;
	std::string hexdigest;
	int i = 0;
	for (std::map<std::string, std::string>::iterator iter = data_->param_map_.begin();
	       iter != data_->param_map_.end();
	       ++iter,i++) {
		if(i!=0)
			os<<"&";
		os<<iter->first<<"="<<iter->second;
	}
	GetSign(hexdigest);
	os<<"&sign="<<hexdigest;
	content_ = os.str();
	data_change_ = false;
}

}
