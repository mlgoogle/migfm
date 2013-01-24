#ifndef _MIG_SSO_ID_SERVICE_PROVIDER_H__
#define _MIG_SSO_ID_SERVICE_PROVIDER_H__

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "lasso/lasso.h"
#include <string>
namespace mig_sso{

class IDProvider{
public:
    IDProvider();
    virtual ~IDProvider();
    bool InitSSO(std::string& path);
    bool SSOCheckUser(const std::string& request,const std::string& usr,
                      const std::string& pwd,std::string& respones);
public:
    static IDProvider* GetInstance();
    static void FreeInstance();
private:
    static IDProvider*   instance_;

private:
    LassoLogin*  idp_login_;
    LassoServer*  idp_server_login_;
};

}
#endif
