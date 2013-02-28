#include "id_provider.h"
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "storage/dic_serialization.h"
#include "storage/db_serialization.h"
#include "config/config.h"
namespace mig_sso{

IDProvider* IDProvider::instance_ = NULL;

IDProvider::IDProvider(){

    lasso_init();

}

IDProvider::~IDProvider(){
}

IDProvider* 
IDProvider::GetInstance(){

    if(instance_ == NULL){

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

const char* IDProvider::InitSSO_Dump(){
    LassoServer* idp_context;
	char* sso_dump = NULL;
    std::string idp_metadata = path_+ "/idp1-la/metadata.xml";
    std::string idp_private_key_raw = path_ + "/idp1-la/private-key-raw.pem";
    std::string idp_certificate = path_ + "/idp1-la/certificate.pem";
    std::string sp_metdata = path_ + "/sp1-la/metadata.xml";
    std::string sp_private_key_raw = path_ + "/sp1-la/public-key.pem";
    std::string sp_certificate = path_ + "/ca1-la/certificate.pem";
    	
    idp_context = lasso_server_new(idp_metadata.c_str(),
                      idp_private_key_raw.c_str(),
                      NULL,
                      idp_certificate.c_str());
                      
    lasso_server_add_provider(idp_context,
                             LASSO_PROVIDER_ROLE_SP,
                             sp_metdata.c_str(),
                             sp_private_key_raw.c_str(),
                             sp_certificate.c_str());
   sso_dump = lasso_server_dump(idp_context);
   g_object_unref(idp_context);
   
   return sso_dump;
}
/*
bool IDProvider::GetIdpDump(std::string& idp_dump){
     char* id_provider_context_dump = NULL;
     LassoServer* server_context =NULL;
     std::string idp_metadata = path_+ "/idp1-la/metadata.xml";
     std::string idp_private_key_raw = path_ + "/idp1-la/private-key-raw.pem";
     std::string idp_certificate = path_ + "/idp1-la/certificate.pem";
     std::string sp_metdata = path_ + "/sp1-la/metadata.xml";
     std::string sp_private_key_raw = path_ + "/sp1-la/public-key.pem";
     std::string sp_certificate = path_ + "/ca1-la/certificate.pem";

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
    idp_dump.assign(id_provider_context_dump);
    g_object_unref(server_context); 
    return true;
}*/
	
bool
IDProvider::InitSSO(std::string& path){
    bool r = false;
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    if(config==NULL)
        return r;
    std::string idp_dump;
    r = config->LoadConfig(path);
    if(!r)
        return r;
    
    r = base_storage::MemDicSerial::Init(config->mem_list_);
    if(!r){
        MIG_ERROR(USER_LEVEL,"init memcached error");
        return false;
    }
    r = base_storage::MysqlSerial::Init(config->mysql_db_list_);
    if(!r){
        MIG_ERROR(USER_LEVEL,"Init mysql db error");
        return false;
    }
    
    path_ = config->certificate_path_;
    sp_url_ = config->sp_url_;
    idp_url_ = config->idp_url_;
    
    std::string sso_key = "mig_sso";
    char* value = NULL;
    size_t value_len = 0;
    r = base_storage::MemDicSerial::GetString(sso_key.c_str(),sso_key.length(),&value,&value_len);
    if(!r){
        const char* id_provider_context_dump = InitSSO_Dump();
        idp_context_dump_.assign(idp_context_dump_);
        base_storage::MemDicSerial::SetString(sso_key.c_str(),sso_key.length(),id_provider_context_dump,
                                              strlen(id_provider_context_dump)+1);
    }else{
        idp_context_dump_.assign(value,value_len);
    }
  /*  const char* id_provider_context_dump = InitSSO_Dump();
    	
    idp_server_login_ = 
       lasso_server_new_from_dump(id_provider_context_dump);

    idp_login_ = lasso_login_new(idp_server_login_);   */
    return true;
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

    idp_server_login_ = 
       lasso_server_new_from_dump(idp_context_dump_.c_str());

    idp_login_ = lasso_login_new(idp_server_login_);
    
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
        authentication_result = 1;
    else 
        authentication_result = 0;
      
    rc = lasso_login_validate_request_msg(idp_login_,
                                          authentication_result,
                                          0);
   
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
    MIG_DEBUG(USER_LEVEL,"========================================\n");
    MIG_DEBUG(USER_LEVEL,"session[%s]",idp_session_context_dump);
    MIG_DEBUG(USER_LEVEL,"========================================\n");
    char* service_provider_id = g_strdup(LASSO_PROFILE(idp_login_)->remote_providerID);
    response_url = LASSO_PROFILE(idp_login_)->msg_url;
    response_query = strchr(response_url,'?')+1;
    respones.assign(response_url); 
   

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

   response[len] = '\0'; //tciketi
     
   base_storage::MemDicSerial::IdpCheckSerial(response,idp_identity_context_dump,
                                              idp_session_context_dump,
                                              service_provider_id);
  /* lasso_server_destroy(idp_server_login_);
   lasso_login_destroy(idp_login_);
   g_free(service_provider_id);               
   //g_free(const_cast<char*>(id_provider_context_dump));
   g_free(idp_session_context_dump);
   g_free(idp_identity_context_dump);*/
   return true;

}

bool IDProvider::SSOIdpCheck(const std::string& sp_soap_msg,
	                         std::string& idp_soap_msg){
    LassoServer *idp_context;
    LassoLogin *idp_login_context;
    std::string identity;
    std::string session;
    std::string provider;
    int32 rc = 0;
    bool r = false;
    int32 request_type = lasso_profile_get_request_type_from_soap_msg(sp_soap_msg.c_str());
    /*if(request_type==LASSO_REQUEST_TYPE_LOGIN){
        MIG_ERROR(USER_LEVEL,"lasso_profile_get_request_type_from_soap_msg errnor");
        return false;
    }*/

    const char* id_provider_context_dump = idp_context_dump_.c_str();
    
    idp_context = lasso_server_new_from_dump(id_provider_context_dump);
    idp_login_context = lasso_login_new(idp_context);
    
    rc = lasso_login_process_request_msg(idp_login_context,(gchar*)sp_soap_msg.c_str());
    
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_process_request_msg errno");
        return false;
    }
    
    r = base_storage::MemDicSerial::IdpCheckUnserial(idp_login_context->assertionArtifact,identity,
    							                     session,provider);

    
    MIG_DEBUG(USER_LEVEL,"identity[%s]",identity.c_str());
    MIG_DEBUG(USER_LEVEL,"session[%s]",session.c_str());
    MIG_DEBUG(USER_LEVEL,"provider[%s]",provider.c_str());
    
    if(!r)
    	return false;
    rc = lasso_profile_set_session_from_dump(LASSO_PROFILE(idp_login_context),session.c_str());
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_frofile_set_session_from_dump:[%d]",rc);
        g_object_unref(idp_login_context);
        g_object_unref(idp_context);
        return false;
    }
    
    rc = lasso_login_build_response_msg(idp_login_context,(gchar*)provider.c_str());
    if(rc!=0){
        MIG_ERROR(USER_LEVEL,"lasso_login_build_response_msg[%d]",rc);
        g_object_unref(idp_login_context);
        g_object_unref(idp_context);
        return false;
    }
    base_storage::MemDicSerial::DeleteIdpCheck(idp_login_context->assertionArtifact);
    idp_soap_msg.assign(LASSO_PROFILE(idp_login_context)->msg_body);
    //g_free(const_cast<char*>(id_provider_context_dump));
    /*g_object_unref(idp_context);
    g_object_unref(idp_login_context);*/
    return true;
}

}
