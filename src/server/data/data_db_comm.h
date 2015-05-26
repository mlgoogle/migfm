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

#include "storage_base_engine.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "logic/logic_infos.h"
#include <string>

namespace base_logic{


class DataDBComm{
public:
	DataDBComm();
	virtual ~DataDBComm();
public:
	bool ReadUserInfo(const int64 uid,base_logic::UserInfo& info);
public:
	static void CallBackReadUserInfo(void* param,base_logic::Value* value);
private:
	scoped_ptr<base_logic::DataStorageBaseEngine> mysql_engine_;
};
/*
class DataDBComm{
public:
	DataDBComm();
	virtual ~DataDBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
	static bool BatchGetUserInfo(std::list<int64> batch_uid,
			std::map<int64,base_logic::UserInfo>& info_map);
};
*/

}



#endif /* DB_COMM_H_ */
