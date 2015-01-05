/*
 * block_msg_queue.h
 *
 *  Created on: 2015年1月3日
 *      Author: kerry
 */

#ifndef BASE_LOGIC_BLOCK_MSG_QUEUE_H_
#define BASE_LOGIC_BLOCK_MSG_QUEUE_H_

#include "logic/base_values.h"
#include "basic/basic_info.h"
#include "basic/scoped_ptr.h"
#include <list>
namespace base_queue{

enum SerializerType{
	TYPE_JSON = 0,
	TYPE_XML = 1,
	TYPE_HTTP = 2
};


class BlockMsg:public base_logic::DictionaryValue{
public:
	BlockMsg(){
		formate_type_ = 0;
		message_list_.reset(new base_logic::ListValue());
	}

	inline  void AddBlockMsg(base_queue::BlockMsg* value){
		this->message_list_->Append(value);
	}

	inline void SetFormate(const int64 formate_type){
		this->formate_type_ = formate_type;
	}

	inline void SetName(const std::string& name){
		this->name_ = name;
	}

	inline void SetMsgType(const int64 msg_type){
		this->msg_type_ = msg_type;
	}

	base_queue::BlockMsg* release(){
		this->Set(L"list",message_list_.release());
		this->SetBigInteger(L"formate",formate_type_);
		this->SetString(L"name",name_);
		return this;
	}

private:
	int64                                         formate_type_;
	std::string                                   name_;
	std::string                                   msg_type_;
	scoped_ptr<base_logic::ListValue>             message_list_;
};


class MsgSerializer{
public:
	static MsgSerializer* Create(int32 type);
	virtual ~MsgSerializer(){};
public:
	virtual bool Serialize(const base_queue::BlockMsg* value,std::string* str) = 0;
};

class JsonMsgSerializer:public MsgSerializer{
public:
	JsonMsgSerializer();
	~JsonMsgSerializer();
public:
	virtual bool Serialize(const base_queue::BlockMsg* value,std::string* str);
};


class BaseBlockMsgQueue{ //初始化连接队列 放入redis
public:
	BaseBlockMsgQueue();
	~BaseBlockMsgQueue();
public:
	bool Init(std::list<base::ConnAddr>& addrlist);
	bool AddBlockMsgQueue(base_queue::BlockMsg* value);

private:
	bool AddBlockMsgQueue(const std::string& key,const int32 type,base_queue::BlockMsg* value);
};



}




#endif /* BLOCK_MSG_QUEUE_H_ */
