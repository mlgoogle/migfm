/*
 * block_msg_queue.cc
 *
 *  Created on: 2015年1月3日
 *      Author: kerry
 */

#include "block_msg_queue.h"
#include "queue_dic_comm.h"
#include "basic/scoped_ptr.h"
#include <sstream>
namespace base_queue{



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

}


