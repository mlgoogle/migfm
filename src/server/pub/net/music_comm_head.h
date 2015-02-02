/*
 * music_comm_head.h
 *
 *  Created on: 2015年2月2日
 *      Author: pro
 */

#ifndef MUSIC_COMM_HEAD_H_
#define MUSIC_COMM_HEAD_H_

#include "net/comm_head.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "logic/base_values.h"
#include <list>
#include <string>
#include <sstream>
namespace netcomm_recv{

class Collect:public LoginHeadPacket{
public:
	Collect(NetBase* m)
	:LoginHeadPacket(m){
		Init();
	}

	inline void Init(){
		bool r = false;
#if defined(__OLD_VERSION__)
		GETBIGINTTOINT(L"taruid",tid_);
#else
		GETBIGINTTOINT(L"tid",tid_);
#endif
	}

	const inline int64 tid() const {return this->tid_;}
private:
	int64  tid_;
};
}

namespace netcomm_send{
class MusicList:public HeadPacket{
public:
	MusicList(){
		base_.reset(new netcomm_send::NetBase());
		list_.reset(new base_logic::ListValue());
	}

	netcomm_send::NetBase* release(){
		if(!list_->empty())
			this->base_->Set(L"song",list_.release());
		head_->Set("result",base_.release());
		this->set_status(1);
		return head_.release();
	}

	inline void set_list(base_logic::DictionaryValue* music){
		list_->Append(music);
	}

private:
	scoped_ptr<netcomm_send::NetBase>             base_;
	scoped_ptr<base_logic::ListValue>             list_;

};


}



#endif /* MUSIC_COMM_HEAD_H_ */
