/*
 * lbs_dic_comm.cc
 *
 *  Created on: 2015年3月6日
 *      Author: pro
 */

#include "lbs_dic_comm.h"
#include "basic/basic_util.h"
#include <sstream>
namespace base_lbs{

base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;


void LocationToKey(const double latitude,const double longitude,std::string& key){
	std::string str_latitude = base::BasicUtil::StringUtil::DoubleToString(latitude);
	std::string str_longitue = base::BasicUtil::StringUtil::DoubleToString(longitude);
	//保留坐标小数点后1位
	size_t star_pos = str_latitude.find(".");
	std::string str_sub_latitude = str_latitude.substr(0,star_pos+2);
	star_pos = str_longitue.find(".");
	std::string str_sub_longitue = str_longitue.substr(0,star_pos+2);

	std::stringstream os;
	os<<"location:"<<str_sub_latitude<<","<<str_sub_longitue;
	key = os.str();
}
void MemComm::Init(std::list<base::ConnAddr>& addrlist){
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	engine_->Connections(addrlist);
}

void MemComm::Dest(){
	if (engine_){
		delete engine_;
		engine_ = NULL;
	}
}

bool MemComm::SetLocation(const double latitude,const double longitude,const char* data,
			size_t len){

	std::string key;
	LocationToKey(latitude,longitude,key);
	return engine_->SetValue(key.c_str(),key.length(),data,len);

}

bool MemComm::GetLocation(const double latitude,const double longitude,char** data,
			size_t* len){
	std::string key;
	LocationToKey(latitude,longitude,key);
	return engine_->GetValue(key.c_str(),key.length(),data,len);
}

bool MemComm::DelLocation(const double latitude,const double longitude){
	std::string key;
	LocationToKey(latitude,longitude,key);
	return engine_->DelValue(key.c_str(),key.length());
}
/*
bool MemComm::SetString(const char* key,const size_t key_len,
        const char* data, size_t len){

}

bool MemComm::GetString(const char* key,const size_t key_len,
        char** data,size_t* len){

}*/
}



