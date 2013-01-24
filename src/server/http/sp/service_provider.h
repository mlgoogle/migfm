#ifndef _MIG_SSO_SP_SERVICE_PROVIDER_H__
#define _MIG_SSO_SP_SERVICE_PROVIDER_H__

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "lasso/lasso.h"
#include <string>
namespace mig_sso{

class ServiceProvider{
public:
    ServiceProvider();
    virtual ~ServiceProvider();
    bool InitSSO(std::string& path);
    const char* InitSSO_Dump();
    bool ServiceProviderRequestInfo(std::string& url,std::string& request_url);
    bool TicketCheck();
private:
    LassoLogin*      sp_login_context_;
    LassoServer*     sp_login_;
    std::string      path_;
public:
    static ServiceProvider* GetInstance();
    static void FreeInstance();
private:
    static ServiceProvider*   instance_;
};

}
#endif
