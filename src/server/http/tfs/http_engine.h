#ifndef _SPIDER_HTTP_ENGINE_H__
#define _SPIDER_HTTP_ENGINE_H__
#include "basic/basictypes.h"
#include "storage/storage.h"
#include <string>
	

namespace spider{
class HttpResponse;
class HttpDownFile;
class SpiderHttpEngine{
public:
	virtual int32 RequestHttp(const std::string& url,std::string& content)=0;
	virtual int32 DownLoadFile(const std::string& url,const std::string&local_file)=0;
};


class SpiderHttpEngineImpl:public SpiderHttpEngine{
public:
    SpiderHttpEngineImpl(base_storage::DBStorageEngine* db_engine);
    virtual ~SpiderHttpEngineImpl();
	int32 GetProxyHost(void);
	int32 UpDateProxyHost();
    int32 RequestHttp(const std::string& url,std::string& content);
	int32 DownLoadFile(const std::string& url,const std::string&local_file);
    int32 Init();
private:
	std::string  host_;
	int          port_;
	int          index_;
	base_storage::DBStorageEngine* db_engine_;
};

}
#endif
