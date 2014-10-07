#include "robot_basic_infos.h"
#include "basic/base64.h"
#include "basic/constants.h"
#include "json/json.h"
#include "log/mig_log.h"
namespace robot_base{

RobotInfo::RobotInfo(){
	data_ = new Data();
}
RobotInfo::RobotInfo(const int64 uid,const std::string& nickname,const std::string& sex,
		const std::string& head_url){
	data_ = new Data(uid,nickname,sex,head_url);
}

RobotInfo::RobotInfo(const RobotInfo& robotinfo)
:data_(robotinfo.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

RobotInfo& RobotInfo::operator =(const RobotInfo& robotinfo){

	if (robotinfo.data_!=NULL){
		robotinfo.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = robotinfo.data_;
	return *this;
}


MailUserInfo::MailUserInfo(){
	data_ = new Data();
}
MailUserInfo::MailUserInfo(const int64 uid,const std::string& username,
			const std::string& nickname){
	data_ = new Data(uid,username,nickname);
}

MailUserInfo::MailUserInfo(const MailUserInfo& mail_userinfo)
:data_(mail_userinfo.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

MailUserInfo& MailUserInfo::operator =(const MailUserInfo& mail_userinfo){

	if (mail_userinfo.data_!=NULL){
		mail_userinfo.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = mail_userinfo.data_;
	return *this;
}

NewMusicInfo::NewMusicInfo(){
    data_ = new Data();
}

NewMusicInfo::NewMusicInfo(const int64 id,const std::string& name,
        const std::string& singer){
	data_ = new Data(id,name,singer);
}

NewMusicInfo::NewMusicInfo(const NewMusicInfo& music_info)
:data_(music_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

NewMusicInfo& NewMusicInfo::operator =(const NewMusicInfo& music_info){
	
	if (music_info.data_!=NULL){
		music_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = music_info.data_;
	return *this;
}

void NewMusicInfo::UnserializedJson(std::string& str){
	Json::Reader reader;
	Json::Value root;
	bool r = false;
	r = reader.parse(str.c_str(),root);
	if (!r){
		MIG_ERROR(USER_LEVEL,"json parser error");
		return;
	}
	//base64 转码
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	Base64Decode(root["titile"].asString(),&b64title);
	Base64Decode(root["artist"].asString(),&b64artist);
	//Base64Decode(smi.album_title(),&b64album);
	set_id(atoll(root["id"].asString().c_str()));
	set_singer(b64artist);
	set_name(b64title);
}

SINAWBAccessToken::SINAWBAccessToken(){
    data_ = new Data();
}

SINAWBAccessToken::SINAWBAccessToken(const std::string& appkey,const std::string& appsecret,
		const std::string& callback,const int32 type){

	data_ = new Data(appkey,appsecret,callback,type);
}

SINAWBAccessToken::SINAWBAccessToken(const SINAWBAccessToken& access_token)
:data_(access_token.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

SINAWBAccessToken& SINAWBAccessToken::operator =(const SINAWBAccessToken& access_token){

	if (access_token.data_!=NULL){
		access_token.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = access_token.data_;
	return *this;
}

bool SINAWBAccessToken::operator < (SINAWBAccessToken& access_token){
	LOG_DEBUG("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	return data_->count_ < access_token.count();
}

bool SINAWBAccessToken::operator > (SINAWBAccessToken& access_token){
	LOG_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	return data_->count_ > access_token.count();
}

bool SINAWBAccessToken::cmp(SINAWBAccessToken* t_access_token,SINAWBAccessToken* r_access_token){
	LOG_DEBUG("cmp");
	return t_access_token->count() < r_access_token->count();
}

void SINAWBAccessToken::Dump(){
	DUMP_STRING(data_->appkey_);
	DUMP_STRING(data_->appsecret_);
	DUMP_STRING(data_->access_token_);
	DUMP_INT(data_->count_);
}

}
