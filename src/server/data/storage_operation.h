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

#include "config/config.h"
#include "logic/logic_infos.h"
namespace base_logic{

class StorageOperation{
public:
	StorageOperation();
	virtual ~StorageOperation();
public:
	static StorageOperation* Instance();
	static void FreeInstance();
	static StorageOperation* instance_;
public:
	void Init(config::FileConfig* config);
public:
	bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
};
}



#endif /* STORAGE_OPERATION_H_ */
