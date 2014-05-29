#ifndef _MASTER_PLUGIN_SSO_IDENTITY_PROIVDER_H__
#define _MASTER_PLUGIN_SSO_IDENTITY_PROIVDER_H__

#include "lasso/lasso.h"
#include <string>
namespace sso_logic{

class IdentityProvider{
public:
	IdentityProvider();
	virtual ~IdentityProvider();
	bool Init();
	const char* InitSSOIDP();

	bool SSOCheckUser(const std::string& request,const std::string& usr,
		const std::string& pwd,std::string& respones);

	bool SSOIdpCheck(const std::string& sp_soap_msg,std::string& idp_soap_msg);

public:
	static IdentityProvider* GetInstance();
	static void FreeInstance();
private:
	static IdentityProvider*   instance_;

private:
	std::string      path_;
	std::string      sp_url_;
	std::string      idp_url_;
	std::string      idp_context_dump_;
};

}
#endif