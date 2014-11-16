#ifndef _MIG_FM_BASE_HTTP_METHOD_H__
#define _MIG_FM_BASE_HTTP_METHOD_H__

#include <string>
#include <vector>
#include <map>
#include "curl/curl.h"
#include "basic/basictypes.h"
#if defined (GOOGLE_URL)
typedef GURL            MIG_URL;
#else
typedef std::string     MIG_URL;
#endif

namespace http{
class HttpMethodGet{
public:

	HttpMethodGet(const MIG_URL& url);
    virtual ~HttpMethodGet(void);
    bool Get(const int port = 0,bool on_error = true);
    const MIG_URL& GetUrl(void){return url_;}
    bool GetContent(std::string& content);
    void SetHeaders(std::string& value);
    void SetResolve(std::string& value);
    inline int  GetCode() {return code_;}
    bool GetHeader(const std::string& key,std::string& value);
private:
	const MIG_URL& url_;

	int code_;
	std::vector<char> content_;
	std::map<std::string,std::string> header_;
	struct curl_slist* headers_;
	struct curl_slist* resolves_;
};

class HttpMethodPost{
public:
	HttpMethodPost(const MIG_URL& url);
    virtual ~HttpMethodPost(void){}
    bool Post(const char* post,const int port = 0,bool on_error = true);
    inline int  GetCode() {return code_;}
    bool GetContent(std::string& content);
    void SetHeaders(std::string& value);
private:
	const MIG_URL& url_;

	int code_;
	std::vector<char> content_;
	std::map<std::string,std::string> header_;
	struct curl_slist* headers_;
	struct curl_slist* bodys_;
};	
}

#endif
