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
    //bool GetIdpDump(std::string& idp_dump);
    const char* InitSSO_Dump();
    bool InitSSO(std::string& path);
    bool SSOCheckUser(const std::string& request,const std::string& usr,
                      const std::string& pwd,std::string& respones);
    bool SSOIdpCheck(const std::string& sp_soap_msg,std::string& idp_soap_msg);
public:
    static IDProvider* GetInstance();
    static void FreeInstance();
private:
    static IDProvider*   instance_;

private:
	std::string    path_;
	std::string    sp_url_;
	std::string    idp_url_;
    LassoLogin*    idp_login_;
    LassoServer*   idp_server_login_;
	std::string    idp_context_dump_;
};

}
#endif
