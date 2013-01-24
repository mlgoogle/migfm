#include "service_provider.h"
#include "log/mig_log.h"
#include "basic/basictypes.h"
namespace mig_sso{

ServiceProvider* ServiceProvider::instance_ = NULL;

ServiceProvider::ServiceProvider(){

    lasso_init();

}

ServiceProvider::~ServiceProvider(){
    g_object_unref(sp_login_context_);
}

ServiceProvider* 
ServiceProvider::GetInstance(){

    if(instance_ = NULL){

        instance_ = new ServiceProvider();
    }

    return instance_;

}

void
ServiceProvider::FreeInstance(){

    if(instance_==NULL){

        delete instance_;
        instance_ =NULL;
    }
}

const char*
ServiceProvider::InitSSO_Dump(){
    LassoServer* sp_context;
    std::string sp_metadata = path_ + "sp1-la/metadata.xml";
    std::string sp_private_key_raw = path_ + "/sp1-la/private-key-raw.pem";
    std::string sp_certificate = path_ + "sp1-la/certificate.pem";
    std::string idp_metadata = path_ + "/idp1-la/metadata.xml";
    std::string idp_private_key_raw = path_ + "/idp1-la/metadata.xml";
    std::string idp_certificate = path_ + "/idp1-la/metadata.xml";
    sp_context  = lasso_server_new(sp_metadata.c_str(),
                                   sp_private_key_raw.c_str(),
                                   NULL,
                                   sp_certificate.c_str());
                                   
    lasso_server_add_provider(sp_context,
                              LASSO_PROVIDER_ROLE_IDP,
                              idp_metadata.c_str(),
                              idp_private_key_raw.c_str(),
                              idp_certificate.c_str());

    return lasso_server_dump(sp_context);
}

bool
ServiceProvider::InitSSO(std::string& path){
    path_ = path;
    char* service_provider_context_dump = InitSSO_Dump(); 
    if(service_provider_context_dump==NULL){
        MIG_ERROR(USER_LEVEL,"servive_provider_context_dump null");
        return false;
    }

    MIG_DEBUG(USER_LEVEL,"service_provider_context_dump[%s]",
              service_provider_context_dump);

    sp_login_ = lasso_server_new_from_dump(service_provider_context_dump);
    sp_login_context_ = lasso_login_new(sp_login_);
    g_object_unref(sp_context);
    return true; 
}

bool ServiceProvider::ServiceProviderRequestInfo(std::string& url,std::string& request_url){
    LassoLibAnthnRequest* request;
    int rc = 0;
    char *relay_state = "fake[]";
    int32 rc = lasso_login_init_authn_request(sp_login_context_,url.c_str(),
                    LASSO_HTTP_METHOD_REDIRECT);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_init_authn_request errno");
        return false;	
    }
    request = LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(sp_login_context_)->request);
    assert(LASSO_IS_LIB_AUTHN_REQUEST(request));
    
    request->IsPassive = 0;
    request->NameIDPolicy = g_strdup(LASSO_LIB_NAMEID_POLICY_TYPE_FEDERATED);
    request->consent = g_strdup(LASSO_LIB_CONSENT_OBTAINED);
    request->RelayState = g_stdup(relay_state);
    rc = lasso_login_build_authn_request_msg(sp_login_context_);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_build_authn_request_msg errno");
        return false;	
    }
    
    request_url.assign(LASSO_PROFILE(spLoginContext)->msg_url);
    return true;
}

bool ServiceProvider::TicketCheck(){
    int32 rc = 0;
    char* respone = NULL;
    rc = lasso_login_init_request(sp_login_context_,respone,LASSO_HTTP_METHOD_POST);
    if(rc!=0){
        assert(rc==0)
        MIG_ERROR(USER_LEVEL,"lasso_login_init_request erron");
        return false;
    }
    rc = lasso_login_build_request_msg(sp_login_context_);
    if(rc!=0){
        assert(rc==0);
        MIG_ERROR(USER_LEVEL,"lasso_login_build_request_msg erron");
        return false;
    }
    
    //post curl
    
}
}
