/*
 * soc_basic_infos.h
 *
 *  Created on: 2014年12月10日
 *      Author: kerry
 */

#ifndef SOC_BASIC_INFOS_H_
#define SOC_BASIC_INFOS_H_

#include "basic/basictypes.h"
#include <string>
#include <list>
namespace socsvc_logic{


class BarrageInfos{
public:
	explicit BarrageInfos();
	explicit BarrageInfos(const int64 msgid,const int64 fid,const std::string& nickname,
			const std::string& message);

	BarrageInfos(const BarrageInfos& barrageinfo);
	BarrageInfos& operator = (const BarrageInfos& barrageinfo);

	const int64 msg_id(){return data_->msgid_;}
	const int64 fid(){return data_->fid_;}
	const std::string& nickname(){return data_->nickname_;}
	const std::string& message(){return data_->message_;}

	void set_msg_id(const int64 msg_id) {data_->msgid_ = msg_id;}
	void set_fid(const int64 fid) {data_->fid_ = fid;}
	void set_nickname(const std::string& nickname){data_->nickname_ = nickname;}
	void set_message(const std::string& message){data_->message_ = message;}
private:
	class Data{
	public:
		Data()
		  :refcount_(1)
		  ,msgid_(0)
		  ,fid_(0){}
		Data(const int64 msg_id,const int64 fid,const std::string& nickname,
				const std::string& message)
		:refcount_(1)
		,msgid_(msg_id)
		,fid_(fid)
		,nickname_(nickname)
		,message_(message){}

	public:
		int64 msgid_;
		int64 fid_;
		std::string nickname_;
		std::string message_;
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	private:
		int refcount_;
	};
	Data*       data_;
};


}


#endif /* SOC_BASIC_INFOS_H_ */
