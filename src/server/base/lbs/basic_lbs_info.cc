#include "basic_lbs_info.h"

namespace base_lbs{

BaiDuAccessKey::BaiDuAccessKey(){
	data_ = new Data();
}

BaiDuAccessKey::BaiDuAccessKey(const std::string& access_key){
	data_ = new Data(access_key);
}

BaiDuAccessKey::BaiDuAccessKey(const BaiDuAccessKey& access_key_info)
:data_(access_key_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

BaiDuAccessKey& BaiDuAccessKey::operator =(const BaiDuAccessKey& access_key_info){

	if (access_key_info.data_!=NULL){
		access_key_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = access_key_info.data_;
	return *this;
}

}
