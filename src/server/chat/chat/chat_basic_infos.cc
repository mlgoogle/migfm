#include "chat_basic_infos.h"
#include "basic/constants.h"
#include "json/json.h"
#include "log/mig_log.h"
namespace chat_base{

UserInfo::UserInfo(){
	data_ = new Data();
}

UserInfo::UserInfo(const int64 platform_id,const int64 user_id, 
				   const int64 nicknumber,const int socket,const int64 session,
				   const std::string& nickname,const std::string& head_url, 
				   const std::string& platform_token, 
				   const std::string& chat_token){
    data_ = new Data(platform_id,user_id,nicknumber,socket,session,nickname,
					head_url,platform_token,chat_token);
}

UserInfo::UserInfo(const int64 platform_id,const int64 user_id,
					const int64 nicknumber,const std::string& nickname,
					const std::string& head_url){
	data_ = new Data(platform_id,user_id,nicknumber,nickname,head_url);
}

UserInfo::UserInfo(const UserInfo& user_info)
:data_(user_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

UserInfo& UserInfo::operator =(const UserInfo& user_info){

	if (user_info.data_!=NULL){
		user_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = user_info.data_;
	return *this;
}


bool UserInfo::UnserializedJson(const char* str){
	Json::Reader reader;
	Json::Value root;
	bool r = false;
	r = reader.parse(str,root);
	if (!r){
		MIG_ERROR(USER_LEVEL,"json parser error");
		return false;
	}

	int64 platform_id = 0;
	int64 userid = atoll(root["uid"].asString().c_str());
	int64 nicknumber = userid;
	std::string nickname = root["nickname"].asString();
	std::string head_url = root["head"].asString();
	chat_base::UserInfo usrinfo(platform_id,userid,nicknumber,nickname,
								head_url);
	*this = usrinfo;
	return true;
}

PlatformInfo::PlatformInfo(){
    data_ = new Data();
}

PlatformInfo::PlatformInfo(const int64 platform_id,
						   const std::string& platform_name){
	data_ = new Data(platform_id,platform_name);
}

PlatformInfo::PlatformInfo(const PlatformInfo& platform_info)
:data_(platform_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

PlatformInfo& PlatformInfo::operator =(const PlatformInfo& platfrom_info){
	
	if (platfrom_info.data_!=NULL){
		platfrom_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = platfrom_info.data_;
	return *this;
}
}
