#ifndef _MIG_SSO_HTTP_RESPONSE_H__
#define _MIG_SSO_HTTP_RESPONSE_H__

#include <string>
#include <vector>
#include <map>
#include "curl/curl.h"
#include "basic/basictypes.h"
#include "log/mig_log.h"
#if defined (GOOGLE_URL)
typedef GURL            MIG_URL;
#else
typedef std::string     MIG_URL;
#endif

namespace mig_sso{
class HttpResponse{
public:
    HttpResponse(const MIG_URL& url);
    virtual ~HttpResponse(void);
    bool Get(void);
    const MIG_URL& GetUrl(void){return url_;}
    bool GetContent(std::string& content);
private:
	const MIG_URL& url_;

	int code_;
	std::vector<char> content_;
	std::map<std::string,std::string> header_;
};

class HttpPost{
public:
    HttpPost(const MIG_URL& url);
    virtual ~HttpPost(void){}
    bool Post(const char* post);
    bool GetContent(std::string& content);
private:
	const MIG_URL& url_;

	int code_;
	std::vector<char> content_;
	std::map<std::string,std::string> header_;
};	
}

#endif