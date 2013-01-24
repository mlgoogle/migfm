#include "id_provider.h"
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "storage/dic_serialization.h"
#include "storage/db_serialization.h"
namespace mig_sso{

IDProvider* IDProvider::instance_ = NULL;

IDProvider::IDProvider(){

    lasso_init();

}

IDProvider::~IDProvider(){
}

IDProvider* 
IDProvider::GetInstance(){

    if(instance_ = NULL){

        instance_ = new IDProvider();
    }

    return instance_;

}

void
IDProvider::FreeInstance(){

    if(instance_==NULL){

        delete instance_;
        instance_ =NULL;
    }
}


bool
IDProvider::InitSSO(std::string& path){
     char* id_provider_context_dump = NULL;
     LassoServer* server_context =NULL;
     std::string idp_metadata = path+ "/idp1-la/metadata.xml";
     std::string idp_private_key_raw = path + "/idp1-la/private-key-raw.pem";
     std::string idp_certificate = path + "/idp1-la/certificate.pem";
     std::string sp_metdata = path + "/sp1-la/metadata.xml";
     std::string sp_private_key_raw = path + "/sp1-la/public-key.pem";
     std::string sp_certificate = path + "/ca1-la/certificate.pem";

    server_context = lasso_server_new(idp_metadata.c_str(),
                      idp_private_key_raw.c_str(),
                      NULL,
                      idp_certificate.c_str());

    lasso_server_add_provider(server_context,
                             LASSO_PROVIDER_ROLE_SP,
                             sp_metdata.c_str(),
                             sp_private_key_raw.c_str(),
                             sp_certificate.c_str());
   
    id_provider_context_dump = lasso_server_dump(server_context);
    if(id_provider_context_dump==NULL){
        MIG_ERROR(USER_LEVEL,"id_provider_context_dump null");
        return false;
    }

    idp_server_login_ = 
       lasso_server_new_from_dump(id_provider_context_dump);

    idp_login_ = lasso_login_new(idp_server_login_);

    g_object_unref(server_context);    
}


bool IDProvider::SSOCheckUser(const std::string& request,const std::string& usr,
                              const std::string& pwd,std::string& respones){


    int32 rc = 0;
    int32 authentication_result = 0;
    bool r = false;
    char *idp_identity_context_dump,*idp_session_context_dump;
    char *response_url,*response_query; 
    char *temp_saml,saml[128],*temp_response,response[256];
#if defined (_DEBUG)
    LassoProfile* profile;
#endif
    rc = lasso_login_process_authn_request_msg(idp_login_,request.c_str());
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_process_authn_request_msg error");
        assert(rc==0);
        return false;
    }
    
    rc = lasso_login_must_authenticate(idp_login_);
    if(rc!=1){
        MIG_ERROR(USER_LEVEL,"lasso_login_must_authencate error");
        assert(rc==1);
        return false;
    }

    if(idp_login_->protocolProfile!=LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART){
        MIG_ERROR(USER_LEVEL,"protocol_Profile!=LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART");
        assert(idp_login_->protocolProfile==LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART);
        return false;
    }
    
    rc = lasso_login_must_ask_for_consent(idp_login_);
    if(rc){
        MIG_ERROR(USER_LEVEL,"lasso_login_must_ask_for_consent error");
        assert(!rc);
        return false;
    }

   //check usr
    r = base_storage::MysqlSerial::CheckUserPassword(usr.c_str(),pwd.c_str());
   
    if(r)
        rc = 1;
    else 
        rc = 0;
      
    rc = lasso_login_validate_request_msg(idp_login_,
                                          authentication_result,
                                          rc);
   
#if defined (_DEBUG)
    profile = LASSO_PROFILE(idp_login_);
    MIG_DEBUG(USER_LEVEL,"profile->identity:%p",profile->identity);
#endif     
    rc = lasso_login_build_assertion(idp_login_,
                                    LASSO_SAML_AUTHENTICATION_METHOD_PASSWORD,
                                    "FIXME: authenticationInstant",
                                    "FIXME: reauthenticateOnOrAfter",
                                    "FIXME: notBefore",
                                    "FIXME: notOnOrAfter");
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_build_assertion error");
        return false;
    }

    rc = lasso_login_build_artifact_msg(idp_login_, LASSO_HTTP_METHOD_REDIRECT);
   
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_bild_artifact_msg error");
        return false;
    }

   
    idp_identity_context_dump = lasso_identity_dump(LASSO_PROFILE(idp_login_)->identity);
   
    if(idp_identity_context_dump==NULL){
        MIG_ERROR(USER_LEVEL,"idp_identity_context_dump null");
        assert(idp_identity_context_dump!=NULL);
        return false;
    }


    idp_session_context_dump = lasso_session_dump(LASSO_PROFILE(idp_login_)->session);

    if(idp_session_context_dump==NULL){
        MIG_ERROR(USER_LEVEL,"idp_session_context_dump null");
        assert(idp_session_context_dump!=NULL);
        return false;
    }

    char* service_provider_id = g_strdup(LASSO_PROFILE(idp_login_)->remote_providerID);
    response_url = LASSO_PROFILE(idp_login_)->msg_url;
    response_query = strchr(response_url,'?')+1;
    respones.assign(response_url); 
    /*

    temp_saml = strchr(response_query,'=');
   
    if(temp_saml==NULL){
        MIG_ERROR(USER_LEVEL,"temp_saml null");
        assert(temp_saml!=NULL);
        return  false;
    }

   
    int32 len = temp_saml - response_query;
    memcpy(saml,response_query,len);
    saml[len] = '\0';


    temp_saml++;

    temp_response =strchr(temp_saml,'&');

    if(temp_response==NULL){
        MIG_ERROR(USER_LEVEL,"temp_response null");
        assert(temp_response!=NULL);
        return false;
    }


   len = temp_response - temp_saml;

   memcpy(response,temp_saml,len);

   response[len] = '\0'; //tciketi*/
   
   base_storage::MemDicSerial::IdpCheckSerial(idp_identity_context_dump,
                                              idp_session_context_dump,
                                              service_provider_id,
                                              usr.c_str());
   return true;

}

}
