/*
 * robot_basic_info.cc
 *
 *  Created on: 2014年12月17日
 *      Author: kerry
 */

#include "msg_basic_info.h"
namespace msgsvc_logic{
GivingSongInfo::GivingSongInfo(){
	data_ = new Data();
}

GivingSongInfo::GivingSongInfo(const GivingSongInfo& giving_song)
:data_(giving_song.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

GivingSongInfo& GivingSongInfo::operator =(const GivingSongInfo& giving_song){
	if(giving_song.data_!=NULL){
		giving_song.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}

	data_ = giving_song.data_;
	return (*this);
}

GivingSongInfo::GivingSongInfo(base_logic::DictionaryValue* value){
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


