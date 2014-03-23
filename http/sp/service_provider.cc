#include "service_provider.h"
#include "http_response.h"
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include "storage/dic_serialization.h"
namespace mig_sso{

ServiceProvider* ServiceProvider::instance_ = NULL;
/*LassoLogin*  ServiceProvider::sp_login_context_ = NULL;
LassoServer* ServiceProvider::sp_login_ = NULL;
std::string  ServiceProvider::path_ = "./";
std::string  ServiceProvider::idp_url_ = "http://sso.miglab.com";
std::string  ServiceProvider::sp_url_ = "http://sso.miglab.com";*/
ServiceProvider::ServiceProvider(){

    //lasso_init();

}

ServiceProvider::~ServiceProvider(){
    //g_object_unref(sp_login_context_);
}

ServiceProvider* 
ServiceProvider::GetInstance(){

    if(instance_ == NULL){

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
	char* sso_dump = NULL;
    std::string sp_metadata = path_ + "/sp1-la/metadata.xml";
    std::string sp_private_key_raw = path_ + "/sp1-la/private-key-raw.pem";
    std::string sp_certificate = path_ + "/sp1-la/certificate.pem";
    std::string idp_metadata = path_ + "/idp1-la/metadata.xml";
    std::string idp_public_key_raw = path_ + "/idp1-la/public-key.pem";
    std::string idp_certificate = path_ + "/ca1-la/certificate.pem";
    
    sp_context  = lasso_server_new(sp_metadata.c_str(),
                                   sp_private_key_raw.c_str(),
                                   NULL,
                                   sp_certificate.c_str());

    lasso_server_add_provider(sp_context,
                              LASSO_PROVIDER_ROLE_IDP,
                              idp_metadata.c_str(),
                              idp_public_key_raw.c_str(),
                              idp_certificate.c_str());

    sso_dump  = lasso_server_dump(sp_context);
	g_object_unref(sp_context);
	return sso_dump;
}

bool
ServiceProvider::InitSSO(std::string& path){
    //path_ = path;
    bool r = false;
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    if(config==NULL)
        return r;
    r = config->LoadConfig(path);
    if(!r)
        return r;
    r = base_storage::MemDicSerial::Init(config->mem_list_);
    path_ = config->certificate_path_;
    sp_url_ = config->sp_url_;
    idp_url_= config->idp_url_; 
    return true; 
}

bool ServiceProvider::GetRequestInfo(std::string& request_url){
    return ServiceProviderRequestInfo(idp_url_,request_url);
}

bool ServiceProvider::ServiceProviderRequestInfo(std::string& url,std::string& request_url){
    LassoLibAuthnRequest* request;
    int32 rc = 0;
    char *relay_state = "fake[]";
    //rc = lasso_login_init_authn_request(sp_login_context_,url.c_str(),
      //              LASSO_HTTP_METHOD_REDIRECT);
    const char* service_provider_context_dump = InitSSO_Dump(); 
	sp_context_dump_.assign(service_provider_context_dump);
    if(service_provider_context_dump==NULL){
        MIG_ERROR(USER_LEVEL,"servive_provider_context_dump null");
        return false;
    }

    MIG_DEBUG(USER_LEVEL,"service_provider_context_dump[%s]",
              service_provider_context_dump);

    sp_login_ = lasso_server_new_from_dump(service_provider_context_dump);
    sp_login_context_ = lasso_login_new(sp_login_);
    
    rc = lasso_login_init_authn_request(sp_login_context_,"https://idp1/metadata",
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
    request->RelayState = g_strdup(relay_state);
    rc = lasso_login_build_authn_request_msg(sp_login_context_);
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_build_authn_request_msg errno");
        return false;	
    }
    
    request_url.assign(LASSO_PROFILE(sp_login_context_)->msg_url);
   
    lasso_server_destroy(sp_login_);
	lasso_login_destroy(sp_login_context_);
    return true;
}

bool ServiceProvider::TicketCheck(const std::string& url,const std::string& request_url,
                                   std::string& token){
    int32 rc = 0;
    bool r = false;
    mig_sso::HttpPost  http_post(url);
    std::string response;
    std::string saml_art;
    std::string relay_state;
    std::stringstream os_time;
    time_t current_time;
    char* respone = NULL;
    base::BasicUtil::ParserSpPost(request_url,saml_art,relay_state);
    const char* service_provider_context_dump = sp_context_dump_.c_str(); 
    if(service_provider_context_dump==NULL){
        MIG_ERROR(USER_LEVEL,"servive_provider_context_dump null");
        return false;
    }

    MIG_DEBUG(USER_LEVEL,"service_provider_context_dump[%s]",
              service_provider_context_dump);

    sp_login_ = lasso_server_new_from_dump(service_provider_context_dump);
    sp_login_context_ = lasso_login_new(sp_login_);
    
    rc = lasso_login_init_request(sp_login_context_,const_cast<char*>(saml_art.c_str()),
                                  LASSO_HTTP_METHOD_POST);
    

    if(rc!=0){
        assert(rc==0);
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
    http_post.Post(LASSO_PROFILE(sp_login_context_)->msg_body);
    r = http_post.GetContent(response);
   
    MIG_DEBUG(USER_LEVEL,"length[%d]",response.length());
    MIG_DEBUG(USER_LEVEL,"content[%s]",response.c_str()); 
    rc = lasso_login_process_response_msg(sp_login_context_,(gchar*)response.c_str());
    if(rc!=0){
        assert(rc==0);
        MIG_ERROR(USER_LEVEL,"lasso_login_process_response_msg");
        return false;
    }
    
    rc = lasso_login_accept_sso(sp_login_context_);
    if(rc!=0){
        assert(rc==0);
        MIG_ERROR(USER_LEVEL,"lasso_login_accept_sso");
        return false;
    }

    token = saml_art.c_str();
    //write 
    current_time = time(NULL)+60*60*24*2;
    os_time<<current_time;
    base_storage::MemDicSerial::SetString(token.c_str(),
                         token.length(),
                         os_time.str().c_str(),
                         os_time.str().length());
    
    char* mem_value = NULL;
    size_t mem_value_length = 0;
    r = base_storage::MemDicSerial::GetString(token.c_str(),token.length(),
                                               &mem_value,&mem_value_length);
    
    MIG_DEBUG(USER_LEVEL,"timep[%s]",mem_value);
    //g_free(const_cast<char*>(service_provider_context_dump));
    g_object_unref(sp_login_);
    g_object_unref(sp_login_context_);
    return true;
}
}
