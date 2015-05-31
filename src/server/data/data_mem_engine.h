/*
 * dict_comm.h
 *
 *  Created on: 2015年5月24日
 *      Author: Administrator
 */

#ifndef DATA_DICT_COMM_H_
#define DATA_DICT_COMM_H_

#include "storage_base_engine.h"
#include "logic/logic_infos.h"
#include "storage/storage.h"
#include "basic/scoped_ptr.h"

namespace base_logic{

/*
class BaseRedisComm{

};*/


class DataBaseMemEngine{
public:
	DataBaseMemEngine(){}
	virtual ~DataBaseMemEngine(){}
	void Init(std::list<base::ConnAddr>& addrlist);
	void Dest();
public:
	//static base_storage::DictionaryStorageEngine* engine_;
	scoped_ptr<base_logic::DataStorageBaseEngine> mem_engine_;
};

class DataUserMemEngine:public DataBaseMemEngine{
public:
	DataUserMemEngine();
	virtual ~DataUserMemEngine();
	void Init(std::list<base::ConnAddr>& addrlist);
	void Dest();
public:
	bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
	bool SetUserInfo(const int64 uid,base_logic::UserInfo& info);
	bool BatchGetUserInfo(std::vector<int64>& batch_uid,std::map<int64,base_logic::UserInfo>& usrinfo);
};


}



#endif /* DATA_DICT_COMM_H_ */
