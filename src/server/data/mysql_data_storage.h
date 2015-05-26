/*
 * mysql_crawl_wdj_storage.h
 *
 *  Created on: 2015年5月25日
 *      Author: Administrator
 */

#ifndef PLUGINS_DATA_MYSQL_DATA_STORAGE_H_
#define PLUGINS_DATA_MYSQL_DATA_STORAGE_H_

#include "storage_base_engine.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include <mysql.h>
#include <string>

namespace base_logic{

class MysqlDataStorage:public DataStorageBaseEngine{

public:
	MysqlDataStorage(){}
	virtual ~MysqlDataStorage(){}
public:
	void Release();
public:
	bool ReadData(const std::string& sql,base_logic::Value* value,
			void (*storage_get)(void*,base_logic::Value*));

	bool WriteData(const int32 type,base_logic::Value* value);

};

}



#endif /* PLUGINS_CRAWLERSVC_WDJ_MYSQL_CRAWL_WDJ_STORAGE_H_ */
