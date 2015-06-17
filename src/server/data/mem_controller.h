/*
 * mem_data_storage.h
 *
 *  Created on: 2015年5月26日
 *      Author: kerry
 *  Time: 下午11:03:15
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */

#ifndef PLUGINS_DATA_MEM_CONTROLLER_
#define PLUGINS_DATA_MEM_CONTROLLER_

#include "storage_controller_engine.h"
#include "logic/base_values.h"
#include <list>

namespace base_logic{


class MemController:public DataControllerEngine{
public:
	MemController();
	virtual ~MemController(){}
public:
	void InitParam(std::list<base::ConnAddr>& addrlist);
	void Release();//释放
public:
	bool WriteData(const int32 type,base_logic::Value* value) ;

	bool ReadData(const int32 type,base_logic::Value* value,
			void (*storage_get)(void*,base_logic::Value*));
private:
	bool WriteKeyValueData(base_logic::Value* value);
	bool ReadKeyValueData(base_logic::Value* value);
	bool ReadBatchKeyValue(base_logic::Value* value);
private:
	base_storage::DictionaryStorageEngine* engine_;
};
}



#endif /* MEM_DATA_STORAGE_H_ */
