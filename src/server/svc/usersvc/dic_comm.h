/*
 * dic_comm.h
 *
 *  Created on: 2015年3月6日
 *      Author: pro
 */

#ifndef USERSVC_DIC_COMM_H_
#define USERSVC_DIC_COMM_H_

#include "config/config.h"
#include <list>
#include <map>

namespace usersvc_logic{



class UserDicComm{
public:
	UserDicComm();
	~UserDicComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static void GetNewMsgNum(const int64 uid, int32& msg_num);
};


}




#endif /* DIC_COMM_H_ */
