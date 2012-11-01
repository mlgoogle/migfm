#ifndef _BUGLE_FILE_PLUGIN_CONFIG_H__
#define _BUGLE_FILE_PLUGIN_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <expat.h>
#include <list>
#include "conn_addr.h"
namespace base{
	
class FileConfig{
public:
	FileConfig();
	virtual ~FileConfig();
public:
	 bool LoadConfig(std::string& path);
public:
	std::list<ConnAddr>  db_list_;
	std::list<ConnAddr>  mem_list_;
	std::list<ConnAddr>  redis_list_;
};

}
#endif
