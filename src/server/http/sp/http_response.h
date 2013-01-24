#ifndef _MIG_SSO_HTTP_RESPONSE_H__
#define _MIG_SSO_HTTP_RESPONSE_H__

namespace mig_sso{
class HttpResponse{
public:
	HttpResponse(const GURL& url);
    virtual ~HttpResponse(void);
    bool Get(void);
    const GURL& GetUrl(void){return url_;}
private:
	const GURL& url_;
	int code_;
};	
}

#endif