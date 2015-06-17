/*
 * mysql_controller.h
 *
 *  Created on: 2015年5月25日
 *      Author: Administrator
 */

#ifndef PLUGINS_DATA_MYSQL_CONTROLLER_
#define PLUGINS_DATA_MYSQL_CONTROLLER_

#include "storage_controller_engine.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include <mysql.h>
#include <string>

namespace base_logic{

class MysqlController:public DataControllerEngine{

public:
	MysqlController(){}
	virtual ~MysqlController(){}
public:

	void InitParam(std::list<base::ConnAddr>& addrlist);
	void Release();
public:
	bool ReadData(const int32 type,base_logic::Value* value,
				void (*storage_get)(void*,base_logic::Value*));

	bool WriteData(const int32 type,base_logic::Value* value);

};

}



#endif /* PLUGINS_CRAWLERSVC_WDJ_MYSQL_CRAWL_WDJ_STORAGE_H_ */
