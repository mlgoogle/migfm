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
	BlockMsg()
	:m_(NULL){
		formate_type_ = 0;
		msg_type_ = 0;
		message_list_.reset(new base_logic::ListValue());
	}

	BlockMsg(base_logic::DictionaryValue* m)
	:m_(m){
		DeserializeInit();
	}

	void DeserializeInit();

	inline  void AddBlockMsg(base_queue::BlockMsg* value){
		this->message_list_->Append(value);
	}

	base_logic::ListValue* message_list(){
		return this->message_list_.release();
	}

	inline void SetFormate(const int64 formate_type){
		this->formate_type_ = formate_type;
	}

	inline void SetName(const std::string& name){
		this->name_ = name;
	}

	const inline int32 MsgType() const {return this->msg_type_;}

	inline void SetMsgType(const int32 msg_type){
		this->msg_type_ = msg_type;
	}


	const inline std::string& Name() const {
		return this->name_;
	}


	base_queue::BlockMsg* release(){
		this->Set(L"list",message_list_.release());
		this->SetBigInteger(L"formate",formate_type_);
		this->SetString(L"name",name_);
		this->SetBigInteger(L"msgtype",this->msg_type_);
		return this;
	}



private:
	int64                                         formate_type_;
	int32                                         msg_type_;
	std::string                                   name_;
	scoped_ptr<base_logic::ListValue>             message_list_;
private:
	base_logic::DictionaryValue*                  m_;
};


class MsgSerializer{
public:
	static MsgSerializer* Create(int32 type);
	virtual ~MsgSerializer(){};
public:
	virtual bool Serialize(const base_queue::BlockMsg* value,std::string* str) = 0;

	virtual base_queue::BlockMsg* Deserialize(std::string& str,int* error_code, std::string* error_str) = 0;
};

class JsonMsgSerializer:public MsgSerializer{
public:
	JsonMsgSerializer();
	~JsonMsgSerializer();
public:
	virtual bool Serialize(const base_queue::BlockMsg* value,std::string* str);

	virtual base_queue::BlockMsg* Deserialize(std::string& str,int* error_code, std::string* error_str);
};


class BaseBlockMsgQueue{ //初始化连接队列 放入redis
public:
	BaseBlockMsgQueue();
	~BaseBlockMsgQueue();
public:
	bool Init(std::list<base::ConnAddr>& addrlist);
	bool AddBlockMsgQueue(base_queue::BlockMsg* value);
	bool GetBlockMsgQueue(const std::string& key,const int32 type,
			std::list<base_queue::BlockMsg*>& list);

private:
	bool AddBlockMsgQueue(const std::string& key,const int32 type,base_queue::BlockMsg* value);
};



}




#endif /* BLOCK_MSG_QUEUE_H_ */
