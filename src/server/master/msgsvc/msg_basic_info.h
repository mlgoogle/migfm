/*
 * rebot_basic_info.h
 *
 *  Created on: 2014年12月17日
 *      Author: kerry
 */

#ifndef MSG_BASIC_INFO_H_
#define MSG_BASIC_INFO_H_

#include "logic/base_values.h"
#include "basic/basictypes.h"
#include <string>
#include <list>
#include <map>
namespace msgsvc_logic{

class GivingSongInfo{
public:
	explicit GivingSongInfo();
	GivingSongInfo(const GivingSongInfo& giving_song);
	GivingSongInfo& operator = (const GivingSongInfo& giving_song);

	GivingSongInfo(base_logic::DictionaryValue* value);

	void set_message(const std::string& message){data_->message_ = message;}
	void set_songid(const std::string& songid){data_->songid_ = songid;}
	void set_tid(const int64 tid){data_->tid_ = tid;}
	void set_uid(const int64 uid){data_->uid_ = uid;}
	void set_msgtype(const int32 msgtype){data_->msgtype_ = msgtype;}
	void set_distance(const double distance) {data_->distance_ = distance;}

	const std::string& message() const {return data_->message_;}
	const std::string& songid() const {return data_->songid_;}
	const int64 tid() const {return data_->tid_;}
	const int64 uid() const {return data_->uid_;}
	const int32 msgtype() const {return data_->msgtype_;}
	const double distance() const {return data_->distance_;}


private:
	class Data{
	public:
		Data()
		:refcount_(0)
		,isadd_(0)
		,msgtype_(0)
		,distance_(0){}
	public:
		int64       tid_;
		int64       uid_;
		int32       isadd_;
		int32       msgtype_;
		double      distance_;
		std::string songid_;
		std::string message_;
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	private:
		int refcount_;
	};
	Data*         data_;
};

}

typedef std::list<msgsvc_logic::GivingSongInfo> GIVINGSONGLIST;
typedef std::map<int64,GIVINGSONGLIST> GIVINGSONGMAP; // uid - LIST
typedef std::map<int64,GIVINGSONGMAP> GIVINGSONGMAPS; //tid - map



#endif /* REBOT_BASIC_INFO_H_ */
