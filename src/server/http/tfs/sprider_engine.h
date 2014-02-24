#ifndef _SPIDER_ENGINE_H__
#define _SPIDER_ENGINE_H__
#include "storage/storage.h"
#include "basic/basic_info.h"
#include "http_engine.h"
#include "fs.h"

namespace spider{

class SpiderEngine{
public:
	SpiderEngine(void);
	virtual ~SpiderEngine(void);
	bool Init(std::list<base::ConnAddr>& addrlist);
	void GetSprider();
private:
	spider::SpiderHttpEngine*        engine_;
	std::string url_;
	base_storage::DBStorageEngine*   mysql_db_engine_;
	filestorage::MFSEngine*          fs_engine_;
	std::string                      nameserver;

};

}
#endif