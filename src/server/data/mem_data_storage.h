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

#ifndef MEM_DATA_STORAGE_H_
#define MEM_DATA_STORAGE_H_

#include "storage_base_engine.h"
#include "logic/base_values.h"
#include <list>

namespace base_logic{

class MemDatalStorage:public DataStorageBaseEngine{
public:
	MemDatalStorage(){}
	virtual ~MemDatalStorage(){}
public:
	void Release();//释放
public:
	bool WriteData(const int32 type,base_logic::Value* value) ;

	bool ReadData(const std::string& sql,base_logic::Value* value,
			void (*storage_get)(void*,base_logic::Value*)){return true;}
private:
	bool WriteKeyValueData(base_logic::Value* value);
};
}



#endif /* MEM_DATA_STORAGE_H_ */
