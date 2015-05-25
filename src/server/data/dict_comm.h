/*
 * dict_comm.h
 *
 *  Created on: 2015年5月24日
 *      Author: Administrator
 */

#ifndef DATA_DICT_COMM_H_
#define DATA_DICT_COMM_H_

#include "logic/logic_infos.h"
#include "storage/storage.h"

namespace base_logic{


class BaseRedisComm{

};


class BaseMemComm{
public:
	BaseMemComm(){}
	virtual ~BaseMemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static base_storage::DictionaryStorageEngine* engine_;
};

class UserMemComm:public BaseMemComm{
public:
	UserMemComm();
	virtual ~UserMemComm();
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
	static bool SetUserInfo(const int64 uid,base_logic::UserInfo& info);
};


}



#endif /* DATA_DICT_COMM_H_ */
