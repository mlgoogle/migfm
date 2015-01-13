/*
 * block_msg_queue.cc
 *
 *  Created on: 2015年1月3日
 *      Author: kerry
 */

#include "block_msg_queue.h"
#include "queue_dic_comm.h"
#include "basic/scoped_ptr.h"
#include "logic/logic_comm.h"
#include <sstream>
namespace base_queue{


void BlockMsg::DeserializeInit(){
	base_logic::ListValue* list;
	int64 formate_type = 0;
	int32 msg_type = 0;
	std::string name;
#define GETBIGINTTOINT(TYPE,VALUE) \
	int64 VALUE##value;\
	m_->GetBigInteger(TYPE,&VALUE##value);\
	VALUE = VALUE##value;\

	GETBIGINTTOINT(L"formate",formate_type);
	GETBIGINTTOINT(L"msgtype",msg_type);
#undef GETBIGINTTOINT
	m_->GetString(L"name",&name);
	m_->GetList(L"list",&list);
	this->formate_type_ = formate_type;
	this->msg_type_ = msg_type;
	this->name_ = name;
	message_list_.swap(list);
}

MsgSerializer* MsgSerializer::Create(int32 type){
	MsgSerializer* engine = NULL;
	switch(type){
	case TYPE_JSON:
		engine = new JsonMsgSerializer();
		break;
	}
	return engine;
}

//JsonMsgSerializer
JsonMsgSerializer::JsonMsgSerializer(){

}
JsonMsgSerializer::~JsonMsgSerializer(){

}

bool JsonMsgSerializer::Serialize(const base_queue::BlockMsg* value,std::string* str){

	scoped_ptr<base_logic::ValueSerializer> serializer(base_logic::ValueSerializer::Create(base_logic::IMPL_JSON,str));
	return serializer->Serialize((*value));
}

base_queue::BlockMsg* JsonMsgSerializer::Deserialize(std::string& str,
		int* error_code, std::string* error_str){

	scoped_ptr<base_logic::ValueSerializer> serializer(base_logic::ValueSerializer::Create(base_logic::IMPL_JSON,&str));
	base_logic::DictionaryValue* dic =  (base_logic::DictionaryValue*)(serializer->Deserialize(error_code,error_str));
	return (new base_queue::BlockMsg(dic));
}





BaseBlockMsgQueue::BaseBlockMsgQueue(){

}

BaseBlockMsgQueue::~BaseBlockMsgQueue(){
	base_queue::QueueDicComm::Dest();
}

bool BaseBlockMsgQueue::Init(std::list<base::ConnAddr>& addrlist){
	base_queue::QueueDicComm::Init(addrlist);
	return true;
}

bool BaseBlockMsgQueue::AddBlockMsgQueue(base_queue::BlockMsg* value){
	int64 type = 0;
	bool r = false;
	std::string key;
	std::string name;
	std::stringstream os;
	r = value->GetBigInteger(L"formate",&type);
	//未设置格式则json
	if(!r) type = TYPE_JSON;
	r = value->GetString(L"name",&name);
	if(!r) name = "unkonw";
	os<<name<<":"<<type;
	//等级
	key = os.str();
	LOG_DEBUG2("key [%s]",key.c_str());
	return AddBlockMsgQueue(key,type,value);
}


bool BaseBlockMsgQueue::AddBlockMsgQueue(const std::string& key,const int32 type,
		base_queue::BlockMsg* value){
	std::string str;
	bool r = false;
	scoped_ptr<base_queue::MsgSerializer> engine(base_queue::MsgSerializer::Create(type));
	r = engine->Serialize(value,&str);
	if(!r)
		return false;
	return base_queue::QueueDicComm::AddBlockQueueMessage(key,str);
}

bool BaseBlockMsgQueue::GetBlockMsgQueue(const std::string& key,const int32 type,
		std::list<base_queue::BlockMsg*>& list){
	bool r = false;
	std::string str;
	std::list<std::string> str_list;

	r = base_queue::QueueDicComm::GetBlockQueueMessage(key,str_list);
	if(!r||str_list.size()<=0)
		return false;
	//遍历解析
	while(str_list.size()>0){
		std::string error_str;
		int error_code = 0;
		str = str_list.front();
		str_list.pop_front();
		scoped_ptr<base_queue::MsgSerializer> engine(base_queue::MsgSerializer::Create(type));
		base_queue::BlockMsg* msg =(base_queue::BlockMsg*) engine->Deserialize(str,&error_code,&error_str);
		if(msg!=NULL){
			list.push_back(msg);
		}
	}
	return r;
}

}


