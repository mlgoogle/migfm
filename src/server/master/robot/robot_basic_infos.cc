#include "robot_basic_infos.h"
#include "basic/constants.h"
#include "json/json.h"
#include "log/mig_log.h"
namespace robot_base{


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

}
