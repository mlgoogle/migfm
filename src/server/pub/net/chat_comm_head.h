/*
 * chat_comm_head.h
 *
 *  Created on: 2014年12月14日
 *      Author: kerry
 */

#ifndef CHAT_COMM_HEAD_H_
#define CHAT_COMM_HEAD_H_

#include "net/comm_head.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "logic/base_values.h"
#include <list>
#include <string>
#include <sstream>
namespace netcomm_recv{

class GetIdleChat:public LoginHeadPacket{
public:
	GetIdleChat(NetBase* m)
	  :LoginHeadPacket(m)
	  ,platform_(0){}

	inline void Init(){
		bool r = false;
		GETBIGINTTOINT(L"platform",platform_);
		if(!r) error_code_ = PLATFORM_LACK;
	}
	const inline int64 platform() const {return this->platform_;}
private:
	int64 platform_;
};

class AloneMessage:public LoginHeadPacket{
public:
	AloneMessage(NetBase*m)
     :LoginHeadPacket(m)
	 ,platform_(0)
	 ,fid_(0)
     ,tid_(0)
     ,from_(0)
     ,count_(0)
     ,msgid_(0){
		Init();
	}

	inline void Init(){
		bool r = false;
		GETBIGINTTOINT(L"platform",platform_);
		if(!r) error_code_ = PLATFORM_LACK;
		GETBIGINTTOINT(L"tid",tid_);
		if(!r) error_code_ = TID_LACK;
		GETBIGINTTOINT(L"msgid",msgid_);
		if(!r) error_code_ = MESSAGE_ID_LACK;
		GETBIGINTTOINT(L"from",from_);
		if(!r) from_ = 0;
		GETBIGINTTOINT(L"count",count_);
		if(!r) count_ =5;
	}

	const inline int64 platform() const {return this->platform_;}
	const inline int64 fid() const {return this->uid_;}
	const inline int64 tid() const {return this->tid_;}
	const inline int64 msgid() const {return this->msgid_;}
	const inline int64 from() const {return this->from_;}
	const inline int64 count() const {return this->count_==0?5:this->count_;}

private:
	int64 platform_;
	int64 fid_;
	int64 tid_;
	int64 from_;
	int64 count_;
	int64 msgid_;
};
class GroupMessage:public LoginHeadPacket{
public:
	GroupMessage(NetBase*m)
	  :LoginHeadPacket(m)
	  ,platform_(0)
	  ,groupid_(0)
	  ,from_(0)
	  ,count_(10)
	  ,msgid_(0){
		Init();
	}

	inline void Init(){
		bool r = false;
		GETBIGINTTOINT(L"platform",platform_);
		if(!r) error_code_ = PLATFORM_LACK;
		GETBIGINTTOINT(L"groupid",groupid_);
		if(!r) error_code_ = GROUP_ID_LACK;
		GETBIGINTTOINT(L"msgid",msgid_);
		if(!r) error_code_ = MESSAGE_ID_LACK;
		GETBIGINTTOINT(L"from",from_);
		if(!r) from_ = 0;
		GETBIGINTTOINT(L"count",count_);
		if(!r) count_ =5;
	}
	const inline int64 platform() const {return this->platform_;}
	const inline int64 groupid() const {return this->groupid_;}
	const inline int64 msgid() const {return this->msgid_;}
	const inline int64 from() const {return this->from_;}
	const inline int64 count() const {return this->count_==0?5:this->count_;}
private:
	int64 platform_;
	int64 groupid_;
	int64 from_;
	int64 count_;
	int64 msgid_;
};

}

namespace netcomm_send{
class AloneMessage:public HeadPacket{
public:
	AloneMessage(){
		alone_msg_.reset(new netcomm_send::NetBase());
		message_.reset(new base_logic::ListValue());
	}

	netcomm_send::NetBase* release(){
		if(!message_->empty())
			this->alone_msg_->Set(L"message",message_.release());
		head_->Set(L"result",alone_msg_.release());
		this->set_status(1);
		return head_.release();
	}

public:
	inline void set_message(const int64 platform,const int64 msgid,const int64 fid,const int64 tid,
			const std::string& nickname,const std::string& head,const std::string& message,
			const std::string& lasttime){
		scoped_ptr<base_logic::DictionaryValue> msginfo(new base_logic::DictionaryValue());
		msginfo->SetBigInteger(L"platform",platform);
		msginfo->SetBigInteger(L"id",msgid);
		msginfo->SetBigInteger(L"fid",fid);
		msginfo->SetBigInteger(L"tid",tid);
		msginfo->SetString(L"nickname",nickname);
		msginfo->SetString(L"msg",message);
		msginfo->SetString("head",head);
		msginfo->SetString(L"time",lasttime);
		message_->Append(msginfo.release());
	}
private:
	scoped_ptr<netcomm_send::NetBase>             alone_msg_;
	scoped_ptr<base_logic::ListValue>             message_;
};


class GroupMessage:public HeadPacket{
public:
	GroupMessage(){
		group_msg_.reset(new netcomm_send::NetBase());
		message_.reset(new base_logic::ListValue());
	}

	netcomm_send::NetBase* release(){
		if(!message_->empty())
			this->group_msg_->Set(L"message",message_.release());
		head_->Set(L"result",group_msg_.release());
		this->set_status(1);
		return head_.release();
	}

public:
	inline void set_message(const int64 platform,const int64 msgid,const int64 fid,const int64 groupid,
			const std::string& nickname,const std::string& head,const std::string& message,const std::string& lasttime){
		scoped_ptr<base_logic::DictionaryValue> msginfo(new base_logic::DictionaryValue());
		msginfo->SetBigInteger(L"platform",platform);
		msginfo->SetBigInteger(L"id",msgid);
		msginfo->SetBigInteger(L"fid",fid);
		msginfo->SetBigInteger(L"groupid",groupid);
		msginfo->SetString(L"nickname",nickname);
		msginfo->SetString(L"msg",message);
		msginfo->SetString("head",head);
		msginfo->SetString(L"time",lasttime);
		message_->Append(msginfo.release());
	}
private:
	scoped_ptr<netcomm_send::NetBase>             group_msg_;
	scoped_ptr<base_logic::ListValue>             message_;
};


}


#endif /* CHAT_COMM_HEAD_H_ */
