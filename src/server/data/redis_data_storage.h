/*
 * redis_crawl_storage.h
 *
 *  Created on: 2015年5月21日
 *      Author: Administrator
 */

#ifndef PLUGINS_CRAWLERSVC_STORAGE_REDIS_DATA_STORAGE_H_
#define PLUGINS_CRAWLERSVC_STORAGE_REDIS_DATA_STORAGE_H_
#include "storage_base_engine.h"
#include "logic/base_values.h"
#include <list>

namespace base_logic{

class RedisDatalStorage:public DataStorageBaseEngine{
public:
	RedisDatalStorage(){}
	virtual ~RedisDatalStorage(){}
public:
	void Release();//释放
	void InitParam(std::list<base::ConnAddr>& addrlist){}
public:
	bool WriteData(const int32 type,base_logic::Value* value);

	bool ReadData(const int32 type,base_logic::Value* value,
				void (*storage_get)(void*,base_logic::Value*));
private:
	bool ReadHashData(base_logic::Value* value,
			void (*storage_get)(void*,base_logic::Value*));
};
}


#endif /* PLUGINS_CRAWLERSVC_STORAGE_REDIS_CRAWL_STORAGE_H_ */
