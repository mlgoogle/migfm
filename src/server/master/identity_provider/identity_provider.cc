#include "identity_provider.h"
#include "config/config.h"
#include "logic_comm.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "basic/basic_util.h"
namespace sso_logic{


IdentityProvider::IdentityProvider(){
	lasso_init();
}

IdentityProvider::~IdentityProvider(){

}


bool IdentityProvider::Init(){

	bool r  = false;
	std::string path = DEFAULT_CONFIG_PATH;
	sso_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}
	r = config->LoadConfig(path);
	storage::DBComm::Init(config->mysql_db_list_);
	storage::MemComm::Init(config->mem_list_);
	storage::RedisComm::Init(config->redis_list_);
	path_ = config->certificate_path_;
	sp_url_ = config->sp_url_;
	idp_url_= config->idp_url_;
	idp_context_dump_ = InitSSOIDP();

}

const char* IdentityProvider::InitSSOIDP(){
	LassoServer* idp_context;
	char* sso_dump = NULL;
	std::string idp_metadata = path_+ "/idp1-la/metadata.xml";
	std::string idp_private_key_raw = path_ + "/idp1-la/private-key-raw.pem";
	std::string idp_certificate = path_ + "/idp1-la/certificate.pem";
	std::string sp_metdata = path_ + "/sp1-la/metadata.xml";
	std::string sp_private_key_raw = path_ + "/sp1-la/public-key.pem";
	std::string sp_certificate = path_ + "/ca1-la/certificate.pem";

	idp_context  = lasso_server_new(idp_metadata.c_str(),
		                            idp_private_key_raw.c_str(),
		                            NULL,
		                            idp_certificate.c_str());

	lasso_server_add_provider(idp_context,
		                      LASSO_PROVIDER_ROLE_SP,
		                      sp_metdata.c_str(),
		                      sp_private_key_raw.c_str(),
		                      sp_certificate.c_str());

	sso_dump  = lasso_server_dump(idp_context);
	g_object_unref(idp_context);
	return sso_dump;
}

bool IdentityProvider::SSOCheckUser(const std::string &request, 
									const std::string &usr, 
									const std::string &pwd, 
									std::string &respones){

	int32 rc = 0;
	int32 authentication_result = 0;
	bool r = false;
	char *idp_identity_context_dump,*idp_session_context_dump;
	char *response_url,*response_query; 
	char *temp_saml,saml[128],*temp_response,response[256];
	LassoServer* idp_server_login;
	LassoLogin* idp_login;
#if defined (_DEBUG)
	LassoProfile* profile;
#endif

	idp_server_login = 
		lasso_server_new_from_dump(idp_context_dump_.c_str());

	idp_login = lasso_login_new(idp_server_login);

	rc = lasso_login_process_authn_request_msg(idp_login,request.c_str());
	if(rc!=0){
		MIG_ERROR(USER_LEVEL,"lasso_login_process_authn_request_msg error");
		assert(rc==0);
		return false;
	}

	rc = lasso_login_must_authenticate(idp_login);
	if(rc!=1){
		MIG_ERROR(USER_LEVEL,"lasso_login_must_authencate error");
		assert(rc==1);
		return false;
	}

	if(idp_login->protocolProfile!=LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART){
		MIG_ERROR(USER_LEVEL,"protocol_Profile!=LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART");
		assert(idp_login->protocolProfile==LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART);
		return false;
	}

	rc = lasso_login_must_ask_for_consent(idp_login);
	if(rc){
		MIG_ERROR(USER_LEVEL,"lasso_login_must_ask_for_consent error");
		assert(!rc);
		return false;
	}

	//check usr
	//r = base_storage::MysqlSerial::CheckUserPassword(usr.c_str(),pwd.c_str());

	if(r)
		authentication_result = 1;
	else 
		authentication_result = 0;

	rc = lasso_login_validate_request_msg(idp_login,
		authentication_result,
		0);

	#if defined (_DEBUG)
	profile = LASSO_PROFILE(idp_login);
	MIG_DEBUG(USER_LEVEL,"profile->identity:%p",profile->identity);
	#endif     
	rc = lasso_login_build_assertion(idp_login,
		LASSO_SAML_AUTHENTICATION_METHOD_PASSWORD,
		"FIXME: authenticationInstant",
		"FIXME: reauthenticateOnOrAfter",
		"FIXME: notBefore",
		"FIXME: notOnOrAfter");
	if(rc!=0){
		MIG_ERROR(USER_LEVEL,"lasso_login_build_assertion error");
		return false;
	}

	rc = lasso_login_build_artifact_msg(idp_login, LASSO_HTTP_METHOD_REDIRECT);

	if(rc!=0){
		MIG_ERROR(USER_LEVEL,"lasso_login_bild_artifact_msg error");
		return false;
	}


	idp_identity_context_dump = lasso_identity_dump(LASSO_PROFILE(idp_login)->identity);

	if(idp_identity_context_dump==NULL){
		MIG_ERROR(USER_LEVEL,"idp_identity_context_dump null");
		assert(idp_identity_context_dump!=NULL);
		return false;
	}


	idp_session_context_dump = lasso_session_dump(LASSO_PROFILE(idp_login)->session);

	if(idp_session_context_dump==NULL){
		MIG_ERROR(USER_LEVEL,"idp_session_context_dump null");
		assert(idp_session_context_dump!=NULL);
		return false;
	}
	MIG_DEBUG(USER_LEVEL,"========================================\n");
	MIG_DEBUG(USER_LEVEL,"session[%s]",idp_session_context_dump);
	MIG_DEBUG(USER_LEVEL,"========================================\n");
	char* service_provider_id = g_strdup(LASSO_PROFILE(idp_login)->remote_providerID);
	response_url = LASSO_PROFILE(idp_login)->msg_url;
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

	//base_storage::MemDicSerial::IdpCheckSerial(response,idp_identity_context_dump,
	//	idp_session_context_dump,
	//	service_provider_id);
	return true;
}

bool IdentityProvider::SSOIdpCheck(const std::string &sp_soap_msg, 
								   std::string &idp_soap_msg){

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

		//r = base_storage::MemDicSerial::IdpCheckUnserial(idp_login_context->assertionArtifact,
			                                         //    identity,session,provider);


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
		//base_storage::MemDicSerial::DeleteIdpCheck(idp_login_context->assertionArtifact);
		idp_soap_msg.assign(LASSO_PROFILE(idp_login_context)->msg_body);
		return true;
}
}