/*
 * storage_engine.h
 *
 *  Created on: 2015年5月21日
 *      Author: Administrator
 */

#ifndef PLUGINS_DATA_STORAGE_STORAGE_BASE_ENGINE_H_
#define PLUGINS_DATA_STORAGE_STORAGE_BASE_ENGINE_H_
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "config/config.h"
#include "storage/storage.h"
#include <list>


enum STORAGETYPE{
	REIDS_TYPE = 0,
	MYSQL_TYPE = 1,
	MEM_TYPE = 2
};

enum MEMCACHE_TYPE{
	KEY_VALUE = 0,
	BATCH_KEY_VALUE = 1,
};


namespace base_logic{

class DataStorageBaseEngine{
public:
	static DataStorageBaseEngine* Create(int32 type);
	static void Init(config::FileConfig* config); //初始化
	static void Dest();
	virtual ~DataStorageBaseEngine(){};
public:
	virtual void Release() = 0;//释放
	virtual void InitParam(std::list<base::ConnAddr>& addrlist) = 0;
public:
	virtual bool ReadData(const int32 type,base_logic::Value* value,
				void (*storage_get)(void*,base_logic::Value*)) = 0;

	virtual bool WriteData(const int32 type,base_logic::Value* value) = 0;
};
}


#endif /* PLUGINS_CRAWLERSVC_STORAGE_STORAGE_BASE_ENGINE_H_ */
