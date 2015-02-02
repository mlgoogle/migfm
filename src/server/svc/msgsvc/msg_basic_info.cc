/*
 * robot_basic_info.cc
 *
 *  Created on: 2014年12月17日
 *      Author: kerry
 */

#include "msg_basic_info.h"
namespace msgsvc_logic{
PushMessageInfo::PushMessageInfo(){
	data_ = new Data();
}

PushMessageInfo::PushMessageInfo(const PushMessageInfo& push_message)
:data_(push_message.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

PushMessageInfo& PushMessageInfo::operator =(const PushMessageInfo& push_message){
	if(push_message.data_!=NULL){
		push_message.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}

	data_ = push_message.data_;
	return (*this);
}

PushMessageInfo::PushMessageInfo(base_logic::DictionaryValue* value){
	data_ = new Data();
	bool  r = false;
	base_logic::DictionaryValue* dic =  value;
	r = dic->GetString(L"message",&data_->message_);
	if(!r)data_->message_ = "来听一下嘛";
	r = dic->GetBigInteger(L"tid",&data_->tid_);
	if(!r){data_->isadd_ = 0; return;}
	r = dic->GetBigInteger(L"uid",&data_->uid_);
	if(!r){data_->isadd_ = 0; return;}
	r = dic->GetString(L"songid",&data_->songid_);
	if(!r){data_->isadd_ = 0; return;}
}


}


