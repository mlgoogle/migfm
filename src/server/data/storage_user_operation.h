/*
 * storage_operation.h
 *
 *  Created on: 2015年5月25日
 *      Author: kerry
 *  Time: 下午10:47:48
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */

#ifndef STORAGE_OPERATION_H_
#define STORAGE_OPERATION_H_
#include "data_mem_engine.h"
#include "data_mysql_engine.h"
#include "basic/scoped_ptr.h"
#include "config/config.h"
#include "logic/logic_infos.h"
namespace base_logic{

class StorageUserOperation{
public:
	StorageUserOperation();
	virtual ~StorageUserOperation();
public:
	static StorageUserOperation* Instance();
	static void FreeInstance();
	static StorageUserOperation* instance_;
public:
	void Init(config::FileConfig* config);
public:
	bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
	bool BatchGetUserInfo(std::vector<int64>& uid_list,std::map<int64,base_logic::UserInfo>& userinfo);
private:
	scoped_ptr<base_logic::DataMysqlEngne>    user_engine_;
	scoped_ptr<base_logic::DataUserMemEngine > mem_engine_;
};
}



#endif /* STORAGE_OPERATION_H_ */
