#ifndef _MIG_SSO_SP_SERVICE_PROVIDER_H__
#define _MIG_SSO_SP_SERVICE_PROVIDER_H__

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "lasso/lasso.h"
#include <string>
#include <sstream>
namespace mig_sso{

class ServiceProvider{
public:
    ServiceProvider();
    virtual ~ServiceProvider();
    bool InitSSO(std::string& path);
    const char* InitSSO_Dump();
    bool GetRequestInfo(std::string& request_url);
    bool ServiceProviderRequestInfo(std::string& url,std::string& request_url);
    bool TicketCheck(const std::string& url,const std::string& request_url,
                     std::string& token);
private:
    LassoLogin*      sp_login_context_;
    LassoServer*     sp_login_;
    std::string      path_;
    std::string      sp_url_;
    std::string      idp_url_;
	std::string      sp_context_dump_;
public:
    static ServiceProvider* GetInstance();
    static void FreeInstance();
private:
    static ServiceProvider*   instance_;
};

}
#endif
