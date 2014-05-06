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
    bool LoadConfig(std::string& path);
public:
    static FileConfig* GetFileConfig();
private:
    static FileConfig*  config_;
public:
    std::list<base::ConnAddr>  mysql_db_list_;
    std::list<base::ConnAddr>  mem_list_;
    std::list<base::ConnAddr>  redis_list_;
    std::list<base::ConnAddr>  mssql_db_list_;
public:
    std::string      certificate_path_; 
    std::string      idp_url_;
    std::string      sp_url_; 
	std::string      mood_path_;
	std::string      style_path_;
	std::string      usr_local_music_path_;
	std::string      host_;
	std::string      port_;
};

}
#endif
