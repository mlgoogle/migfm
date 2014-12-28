/*
 * chat_basic_infos.h
 *
 *  Created on: 2014年12月10日
 *      Author: kerry
 */

#ifndef CHAT_BASIC_INFOS_H_
#define CHAT_BASIC_INFOS_H_

#include "basic/basictypes.h"
#include <string>
#include <list>
namespace chatsvc_logic{

class AloneMessageInfos{
public:
	explicit AloneMessageInfos();
	explicit AloneMessageInfos(const int64 platform,const int64 fid,const int64 tid,
			const int64 msg_id,const std::string& nickname,
			const std::string& message,const std::string& lasttime,const std::string& pic);

	AloneMessageInfos(const AloneMessageInfos& alonemsginfo);
	AloneMessageInfos& operator = (const AloneMessageInfos& alonemsginfo);
	const int64 platform() const {return data_->platform_;}
	const int64 msgid() const {return data_->msgid_;}
	const int64 fid() const {return data_->fid_;}
	const int64 tid() const {return data_->tid_;}
	const std::string& nickname() const {return data_->nickname_;}
	const std::string& message() const {return data_->message_;}
	const std::string& lasttime() const {return data_->lasttime_;}
	const std::string& head() const {return data_->head_;}

	void set_platform(const int64 platform) {data_->platform_ = platform;}
	void set_msgid(const int64 msgid) {data_->msgid_ = msgid;}
	void set_fid(const int64 fid) {data_->fid_ = fid;}
	void set_tid(const int64 tid) {data_->tid_ = tid;}
	void set_nickname(const std::string& nickname){data_->nickname_ = nickname;}
	void set_message(const std::string& message){data_->message_ = message;}
	void set_lasttime(const std::string& lasttime){data_->lasttime_ = lasttime;}
	void set_head(const std::string& head){data_->head_ = head;}

public:
	class Data{
	public:
		Data()
		  :refcount_(1)
		  ,msgid_(0)
		  ,fid_(0)
		  ,tid_(0)
		  ,platform_(0){}
		Data(const int64 platform,const int64 msg_id,const int64 fid,const int64 tid,
				const std::string& nickname,const std::string& message,const std::string& lasttime,
				const std::string& pic)
		:refcount_(1)
		,platform_(platform)
		,msgid_(msg_id)
		,fid_(fid)
		,tid_(tid)
		,nickname_(nickname)
		,message_(message)
		,head_(pic){}

	public:
		int64 platform_;
		int64 msgid_;
		int64 fid_;
		int64 tid_;
		std::string nickname_;
		std::string message_;
		std::string lasttime_;
		std::string head_;
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	private:
		int refcount_;
	};
	Data*       data_;
};


//继承单聊
/*class GroupMessageInfos:public AloneMessageInfos{
public:
	explicit GroupMessageInfos();
	explicit GroupMessageInfos(const int64 platform,const int64 groupid,const int64 msg_id,const int64 fid,const std::string& nickname,
			const std::string& message,const std::string& lasttime,const std::string& pic);
	GroupMessageInfos(const GroupMessageInfos& groupmsginfo);
	GroupMessageInfos& operator = (const GroupMessageInfos& groupmsginfo);
	const int64 groupid() const {return gdata_->group_id_;}
	void set_groupid(const int64 groupid) {gdata_->group_id_ = groupid;}
private:
	class GData{
	public:
		GData()
			:refcount_(1)
			,group_id_(0){}
		GData(const int64 group_id)
			:refcount_(1)
			,group_id_(group_id){}
	public:
		int64 group_id_;
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	private:
		int refcount_;
	};
	GData*       gdata_;
};*/


class GroupMessageInfos{
public:
	explicit GroupMessageInfos();
	explicit GroupMessageInfos(const int64 platform,const int64 groupid,const int64 msg_id,const int64 fid,const std::string& nickname,
			const std::string& message,const std::string& lasttime);

	GroupMessageInfos(const GroupMessageInfos& groupmsginfo);
	GroupMessageInfos& operator = (const GroupMessageInfos& groupmsginfo);

	const int64 platform() const {return data_->platform_;}
	const int64 groupid() const {return data_->group_id_;}
	const int64 msgid() const {return data_->msgid_;}
	const int64 fid() const {return data_->fid_;}
	const std::string& nickname() const {return data_->nickname_;}
	const std::string& message() const {return data_->message_;}
	const std::string& lasttime() const {return data_->lasttime_;}
	const std::string& head() const {return data_->head_;}

	void set_platform(const int64 platform) {data_->platform_ = platform;}
	void set_groupid(const int64 groupid) {data_->group_id_ = groupid;}
	void set_msgid(const int64 msgid) {data_->msgid_ = msgid;}
	void set_fid(const int64 fid) {data_->fid_ = fid;}
	void set_nickname(const std::string& nickname){data_->nickname_ = nickname;}
	void set_message(const std::string& message){data_->message_ = message;}
	void set_lasttime(const std::string& lasttime){data_->lasttime_ = lasttime;}
	void set_head(const std::string& head){data_->head_ = head;}

private:
	class Data{
	public:
		Data()
		  :refcount_(1)
		  ,msgid_(0)
		  ,fid_(0)
		  ,platform_(0)
	      ,group_id_(0){}
		Data(const int64 platform,const int64 groupid,const int64 msg_id,const int64 fid,const std::string& nickname,
				const std::string& message,const std::string& lasttime)
		:refcount_(1)
		,platform_(platform)
		,group_id_(groupid)
		,msgid_(msg_id)
		,fid_(fid)
		,nickname_(nickname)
		,message_(message){}

	public:
		int64 platform_;
		int64 group_id_;
		int64 msgid_;
		int64 fid_;
		std::string nickname_;
		std::string message_;
		std::string lasttime_;
		std::string head_;
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	private:
		int refcount_;
	};
	Data*       data_;
};

}


#endif /* SOC_BASIC_INFOS_H_ */
