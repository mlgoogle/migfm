/*
 * user_basic_info.cc
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#include "user_basic_info.h"
#include "basic/scoped_ptr.h"
namespace usersvc_logic{

UserInfo::UserInfo(){
	data_ = new Data();
}


UserInfo::UserInfo(const UserInfo& userinfo)
:data_(userinfo.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

UserInfo& UserInfo::operator =(const UserInfo& userinfo){
	if(userinfo.data_!=NULL){
		userinfo.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = userinfo.data_;
	return (*this);
}

base_logic::DictionaryValue* UserInfo::Release(){
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	if(data_->uid_!=0)
		dict->SetBigInteger(L"uid",data_->uid_);
	if(data_->sex_!=0){
		dict->SetInteger(L"sex",data_->sex_);
		dict->SetInteger(L"gender",data_->sex_);
	}
	if(data_->machine_!=0)
		dict->SetInteger(L"machine",data_->machine_);
	if(data_->type_!=0)
		dict->SetInteger(L"type",data_->type_);
	if(!data_->session_.empty())
		dict->SetString(L"session",data_->session_);
	if(!data_->imei_.empty())
		dict->SetString(L"imei",data_->imei_);
	if(!data_->nickname_.empty())
		dict->SetString(L"nickname",data_->nickname_);
	if(!data_->city_.empty()){
		dict->SetString(L"city",data_->city_);
		dict->SetString(L"location",data_->city_);
	}
	if(!data_->head_.empty())
		dict->SetString(L"head",data_->head_);
	if(!data_->birthday_.empty())
		dict->SetString(L"birthday",data_->birthday_);
	if(!data_->location_.empty())
		dict->SetString(L"loaction",data_->location_);
	if(!data_->token_.empty())
		dict->SetString(L"token",data_->token_);
	return dict.release();
}

}
