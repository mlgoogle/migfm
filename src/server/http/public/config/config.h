#ifndef _BUGLE_FILE_PLUGIN_CONFIG_H__
#define _BUGLE_FILE_PLUGIN_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <expat.h>
#include <list>
#include "basic/basic_info.h"
namespace config{
	
class FileConfig{
public:
	FileConfig();
	virtual ~FileConfig();
public:
	 bool LoadConfig(std::string& path);
public:
	std::list<base::ConnAddr>  db_list_;
	std::list<base::ConnAddr>  mem_list_;
	std::list<base::ConnAddr>  redis_list_;
};

}
#endif
