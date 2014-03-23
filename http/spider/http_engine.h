#ifndef _SPIDER_HTTP_ENGINE_H__
#define _SPIDER_HTTP_ENGINE_H__
#include "basic/basictypes.h"
#include <string>
	

namespace spider{
class HttpResponse;
class SpiderHttpEngine{
public:
	virtual int32 RequestHttp(const std::string& url)=0; 
};


class SpiderHttpEngineImpl:public SpiderHttpEngine{
public:
    SpiderHttpEngineImpl();
    virtual ~SpiderHttpEngineImpl();
    int32 RequestHttp(const std::string& url);
    int32 Init();
private:
	spider::HttpResponse*   http_;
};

}
#endif
