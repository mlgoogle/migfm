/*
 * db_comm.h
 *
 *  Created on: 2015年5月25日
 *      Author: kerry
 *  Time: 下午11:02:27
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */

#ifndef DATA_DB_COMM_H_
#define DATA_DB_COMM_H_

#include "logic/logic_infos.h"
#include "config/config.h"
#include <list>

namespace base_logic{

class DataDBComm{
public:
	DataDBComm();
	virtual ~DataDBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
};

}



#endif /* DB_COMM_H_ */
