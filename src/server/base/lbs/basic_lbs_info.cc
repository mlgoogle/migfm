#include "basic_lbs_info.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "logic/base_values.h"

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


LBSInfos::LBSInfos(){
	data_ = new Data();
}

LBSInfos::LBSInfos(const std::string& host,const double& latitude,
			const double& longitude,const std::string& city,const std::string& district,
			const std::string& province,std::string& street,time_t current_time){
	data_ = new Data(host,latitude,longitude,city,district,province,street,current_time);
}

LBSInfos::LBSInfos(const LBSInfos& lbs_basic_info)
:data_(lbs_basic_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

LBSInfos& LBSInfos::operator =(const LBSInfos& lbs_basic_info){
	if(lbs_basic_info.data_!=NULL){
		lbs_basic_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = lbs_basic_info.data_;
	return (*this);
}

void LBSInfos::JsonSerializer(std::string* str){
	bool r  = false;
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	if(!data_->host_.empty())
		dict->SetString(L"host",data_->host_);
	if(data_->longitude_!=0.0)
		dict->SetReal(L"longitude",data_->longitude_);
	if(data_->latitude_!=0.0)
		dict->SetReal(L"latitude",data_->latitude_);
	if(!data_->city_.empty())
		dict->SetString(L"city",data_->city_);
	if(!data_->district_.empty())
		dict->SetString(L"district",data_->district_);
	if(!data_->province_.empty())
		dict->SetString(L"province",data_->province_);
	if(!data_->street_.empty())
		dict->SetString(L"street",data_->street_);
	if(!data_->current_time!=0)
		dict->SetBigInteger(L"time",data_->current_time);
	base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(base_logic::IMPL_JSON,str);
	base_logic::Value* value = (base_logic::Value*)dict.release();
	engine->Serialize(*value);
}

void LBSInfos::JsonDeserialize(std::string* str){
	bool r  = false;
	int error_code;
	std::string error_str;
	scoped_ptr<base_logic::ValueSerializer> serializer(base_logic::ValueSerializer::Create(base_logic::IMPL_JSON,str));
	base_logic::DictionaryValue* dic =  (base_logic::DictionaryValue*)(serializer->Deserialize(&error_code,&error_str));
	r = dic->GetString(L"host",&data_->host_);
	r = dic->GetString(L"city",&data_->city_);
	r = dic->GetString(L"district",&data_->district_);
	r = dic->GetString(L"province",&data_->province_);
	r = dic->GetString(L"street",&data_->street_);
	r = dic->GetReal(L"longitude",&data_->longitude_);
	r = dic->GetReal(L"longitude",&data_->longitude_);
}


}
