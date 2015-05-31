/*
 * dict_comm.cc
 *
 *  Created on: 2015年5月24日
 *      Author: Administrator
 */
#include "data_mem_engine.h"
#include "dic/base_dic_redis_auto.h"
#include <string>


static const char *USER_INFO = "userinfo:%lld";

namespace base_logic{

//base_storage::DictionaryStorageEngine* BaseMemComm::engine_ = NULL;

void DataBaseMemEngine::Init(std::list<base::ConnAddr>& addrlist){
	mem_engine_.reset(base_logic::DataStorageBaseEngine::Create(MEM_TYPE));
	mem_engine_->InitParam(addrlist);
	//engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	//engine_->Connections(addrlist);
}

void DataBaseMemEngine::Dest(){
	/*if(mem_engine_){
		delete mem_engine_;
		mem_engine_ =NULL;
	}*/
}

DataUserMemEngine::DataUserMemEngine(){

}

DataUserMemEngine::~DataUserMemEngine(){

}

void DataUserMemEngine::Init(std::list<base::ConnAddr>& addrlist){
	DataBaseMemEngine::Init(addrlist);
}

void DataUserMemEngine::Dest(){
}


bool DataUserMemEngine::GetUserInfo(const int64 uid,base_logic::UserInfo& info){
	bool r = false;
	std::string json;
	base_logic::BinaryValue* binary;
	char key[256] = {0};
	snprintf(key, arraysize(key), USER_INFO, uid);
	scoped_ptr<base_logic::DictionaryValue> dic(new base_logic::DictionaryValue());
	dic->SetString(L"memkey",std::string(key));

	r = mem_engine_->ReadData(KEY_VALUE,(base_logic::Value*)(dic.get()),NULL);
	if(!r)
		return false;

	r = dic->GetBinary(L"memvalue",&binary);
	if(!r)
		return false;
	json.assign(binary->GetBuffer(),binary->GetSize());
	return info.JsonDeSerialization(json);
}


bool DataUserMemEngine::SetUserInfo(const int64 uid,base_logic::UserInfo& info){
	bool r = false;
	std::string json;
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	char key[256] = {0};
	snprintf(key, arraysize(key), USER_INFO, uid);
	r = info.JsonSerialization(json);
	if(!r)
		return r;
	base_logic::BinaryValue* binary = base_logic::BinaryValue::CreateWithCopiedBuffer(json.c_str(),json.length());
	dict->SetString(L"memkey",std::string(key));
	dict->Set(L"memvalue",binary);
	return  mem_engine_->WriteData(KEY_VALUE,(base_logic::Value*)(dict.get()));
}

bool DataUserMemEngine::BatchGetUserInfo(std::vector<int64>& batch_uid,
		std::map<int64,base_logic::UserInfo>& usrinfo){
	//std::vector<const char *> vec_keys;
	//std::vector<size_t> vec_key_lens;
	char** keys = new char*[batch_uid.size()];
	size_t* key_len = new size_t[batch_uid.size()];
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	base_logic::ListValue* list;
	int i = 0;
	while(i<batch_uid.size()){
		char key[256] = {0};
		snprintf(key, arraysize(key), USER_INFO, batch_uid[i]);
		keys[i] =  new char[arraysize(key)];
		memcpy(keys[i],key,256);
		key_len[i] = strlen(keys[i]);
		i++;
	}
	//批量读取
	base_logic::BinaryValue* keys_binary = base_logic::BinaryValue::Create((char*)keys,0);
	base_logic::BinaryValue* keys_len_binary = base_logic::BinaryValue::Create((char*)(key_len),0);
	dict->Set(L"batchmemkeys",keys_binary);
	dict->Set(L"batchmemkeyslen",keys_len_binary);
	dict->SetInteger(L"batchmemkeyscount",batch_uid.size());
	mem_engine_->ReadData(BATCH_KEY_VALUE,(base_logic::Value*)(dict.get()),NULL);

	dict->GetList(L"resultvalue",&list);
	while(list->GetSize()>0){
		base_logic::UserInfo userinfo;
		base_logic::BinaryValue* binary_result_value;
		base_logic::Value* result_value;
		list->Remove(0,&result_value);
		binary_result_value = (base_logic::BinaryValue*)(result_value);
		std::string json(binary_result_value->GetBuffer(),binary_result_value->GetSize());
		userinfo.JsonDeSerialization(json);
		//判断是否是完整UID
		if(userinfo.Isvalid())
			usrinfo[userinfo.uid()] = userinfo;
	}
	int32 j = 0;
	while(j<batch_uid.size()){
		if(keys[j]){
			delete [] keys[j];
			keys[j] = NULL;
		}
		j++;
	}
	if(keys){delete keys;}
	if(key_len){delete key_len;}
	return true;
}

}




