#include <lasso/lasso.h>
#include <glib.h>
#include <assert.h>
#include <sstream>
#include "config/config.h"
#include "storage/dic_serialization.h"
#include "json/json.h"
#include "log/mig_log.h"
static char*
generateIdentityProviderContextDump()
{
	LassoServer *serverContext;
	char *ret;

	serverContext = lasso_server_new(
			"/root/kerry/sso/lasso-2.3.6/tests/data" "/idp1-la/metadata.xml",
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/idp1-la/private-key-raw.pem",
			NULL, /* Secret key to unlock private key */
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/idp1-la/certificate.pem");
	lasso_server_add_provider(
			serverContext,
			LASSO_PROVIDER_ROLE_SP,
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/sp1-la/metadata.xml",
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/sp1-la/public-key.pem",
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/ca1-la/certificate.pem");
	ret = lasso_server_dump(serverContext);

	g_object_unref(serverContext);

	return ret;
}

static char*
generateServiceProviderContextDump()
{
	LassoServer *serverContext;
	char *ret;

	serverContext = lasso_server_new(
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/sp1-la/metadata.xml", /* metadata数据-LECP服务器*/
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/sp1-la/private-key-raw.pem",/*private key*/
			NULL, /* Secret key to unlock private key */
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/sp1-la/certificate.pem");
	lasso_server_add_provider(
			serverContext,
			LASSO_PROVIDER_ROLE_IDP,
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/idp1-la/metadata.xml",
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/idp1-la/public-key.pem",
			"/root/kerry/sso/lasso-2.3.6/tests/data"  "/ca1-la/certificate.pem");

	ret = lasso_server_dump(serverContext);
	g_object_unref(serverContext);
	return ret;
}

int main(int argc, char *argv[])
{
	char *serviceProviderContextDump, *identityProviderContextDump;
	LassoServer *spContext, *idpContext;
	LassoLogin *spLoginContext, *idpLoginContext;
	LassoLibAuthnRequest *request;
	int rc;
        bool r = false;
	char *relayState;
	char *authnRequestUrl, *authnRequestQuery;
	char *responseUrl, *responseQuery;
	char *idpIdentityContextDump, *idpSessionContextDump;
	char *serviceProviderId, *soapRequestMsg, *soapResponseMsg;
	char *spIdentityContextDump;
	char *spSessionDump;
	char *spLoginDump;
	int requestType;
	char *found;
	char *TempSaml;
	char Saml[128];
	char *TempResponse;
	char Response[256];
	int len;
        Json::Reader reader;
        Json::Value root;
        std::string path = "./config.xml";
	lasso_init();

        config::FileConfig* config = config::FileConfig::GetFileConfig();

        r = config->LoadConfig(path);

        r = base_storage::MemDicSerial::Init(config->mem_list_);
	//sp
	serviceProviderContextDump = generateServiceProviderContextDump();
        //printf("serviceProviderContextDump[%s]\n\n\n\n\n",serviceProviderContextDump);

	spContext = lasso_server_new_from_dump(serviceProviderContextDump);

	spLoginContext = lasso_login_new(spContext);
	assert(spLoginContext!=NULL);

	rc = lasso_login_init_authn_request(spLoginContext, "https://idp1/metadata",
		LASSO_HTTP_METHOD_REDIRECT); //初始化一个请求，交与IDP,并选择提交IDP的方式
	assert(rc==0);

	request = LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(spLoginContext)->request);
	assert(LASSO_IS_LIB_AUTHN_REQUEST(request));

	request->IsPassive = 0;
	request->NameIDPolicy = g_strdup(LASSO_LIB_NAMEID_POLICY_TYPE_FEDERATED); // NameIDPolicy =  "federated"
	request->consent = g_strdup(LASSO_LIB_CONSENT_OBTAINED);//consent = "urn:liberty:consent:obtained"
	relayState = "fake[]";
	request->RelayState = g_strdup(relayState);
	rc = lasso_login_build_authn_request_msg(spLoginContext);  //产生一个重定向地址(指向IDP地址),存储于msg_url
	assert(rc==0);
	authnRequestUrl = LASSO_PROFILE(spLoginContext)->msg_url;
	assert(authnRequestUrl!=NULL);
	authnRequestQuery = strchr(authnRequestUrl, '?')+1;
	assert(strlen(authnRequestQuery)>0);
	spLoginDump = lasso_node_dump(LASSO_NODE(spLoginContext));

        //printf("authnRequestUrl[%s]\n",authnRequestUrl);

	assert(strstr(authnRequestQuery, "RelayState")!=NULL);
	assert(strstr(authnRequestQuery, "fake%5B%5D")!=NULL);



	
	//IDP

	/* Identity provider singleSignOn, for a user having no federation. */
	identityProviderContextDump = generateIdentityProviderContextDump();
	idpContext = lasso_server_new_from_dump(identityProviderContextDump);
	idpLoginContext = lasso_login_new(idpContext);
	assert(idpLoginContext!=NULL);
	
	rc = lasso_login_process_authn_request_msg(idpLoginContext, authnRequestQuery); //在IDP服务器中认证URL的请求 0
	assert(rc==0);

	rc = lasso_login_must_authenticate(idpLoginContext); 
	assert(rc==1);

	//assert(lasso_login_must_authenticate(idpLoginContext));
	assert(idpLoginContext->protocolProfile==LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART); 

	rc = lasso_login_must_ask_for_consent(idpLoginContext);
	assert(!rc);
	//assert(!lasso_login_must_ask_for_consent(idpLoginContext)); 
	assert(idpLoginContext->parent.msg_relayState!=NULL);
	//rc = g_strcmp0(idpLoginContext->parent.msg_relayState,relayState);
	//assert(g_strcmp0(idpLoginContext->parent.msg_relayState, relayState)==0); //认证返回状态


	//用户名，密码验证


	rc = lasso_login_validate_request_msg(idpLoginContext,
		1,  //验证成功后设置为1
		0);  //响应登录请求

	rc = lasso_login_build_assertion(idpLoginContext,
		LASSO_SAML_AUTHENTICATION_METHOD_PASSWORD,
		"FIXME: authenticationInstant",
		"FIXME: reauthenticateOnOrAfter",
		"FIXME: notBefore",
		"FIXME: notOnOrAfter");
	//产生票据存入到缓存中，用于验证。
	rc = lasso_login_build_artifact_msg(idpLoginContext, LASSO_HTTP_METHOD_REDIRECT); //产生一个请求消息到SP中，并将重定向URL保存在msg_url中

	assert(rc==0);

	idpIdentityContextDump = lasso_identity_dump(LASSO_PROFILE(idpLoginContext)->identity);
	assert(idpIdentityContextDump!=NULL);
	idpSessionContextDump = lasso_session_dump(LASSO_PROFILE(idpLoginContext)->session);
	assert(idpSessionContextDump!=NULL);
  
        char* providerid = g_strdup(LASSO_PROFILE(idpLoginContext)->remote_providerID);
      
        //printf("\n\n====================\nidenttity[%s]\n",idpIdentityContextDump);

        //printf("session[%s]\n",idpSessionContextDump);

        //printf("provider id[%s]\n=======================\n",providerid);

	responseUrl = LASSO_PROFILE(idpLoginContext)->msg_url;
	assert(responseUrl!=NULL);
	responseQuery = strchr(responseUrl, '?')+1;
        //printf("\nresponseUrl[%s]\n",responseUrl);

	//解析票据
	TempSaml = strchr(responseQuery,'=');
	if (TempSaml == NULL)
	{
	}
	len = TempSaml - responseQuery;
	memcpy(Saml,responseQuery,len);
	Saml[len] = '\0';
	
	TempSaml++;
	TempResponse = strchr(TempSaml,'&');
	if (TempResponse == NULL)
	{
	}
	len =TempResponse - TempSaml;
	memcpy(Response,TempSaml,len);
	Response[len] = '\0'; //获取票据

	////
	assert(strlen(responseQuery)>0);
	assert(strstr(responseQuery, "RelayState")!=NULL);
	assert(strstr(responseQuery, "fake%5B%5D")!=NULL);
	serviceProviderId = g_strdup(LASSO_PROFILE(idpLoginContext)->remote_providerID);
	assert(serviceProviderId!=NULL);

        //printf("\nTempSaml[%s]\n",TempSaml);

        //printf("\nResponse[%s]\n",Response);

        //printf("\nresponseQuery[%s]\n",responseQuery);
//如果已经登录过	验证用户过程

	/* Service provider assertion consumer */
        time_t time1 = time(NULL);
        for(int t = 0;t<1;t++){
        std::string idp_check_key = "idp_1009";
        std::stringstream os;
        MIG_DEBUG(USER_LEVEL,"=================================\n");
        MIG_DEBUG(USER_LEVEL,"session[%s]",idpSessionContextDump);
        MIG_DEBUG(USER_LEVEL,"==================================\n");
        
        r = base_storage::MemDicSerial::SetString(idp_check_key.c_str(),idp_check_key.length(),
                                                  idpSessionContextDump,strlen(idpSessionContextDump)+1);

        os<<"{\"ident\":"<<"\""<<idpIdentityContextDump<<"\",\n\n\"session\":\""<<idpSessionContextDump<<"\",\"provider\":\""
          <<serviceProviderId<<"\",\n\n\"respone\":\""<<Response<<"\"}";
        //r = base_storage::MemDicSerial::SetString(idp_check_key.c_str(),idp_check_key.length(),
        //                                                  os.str().c_str(),os.str().length());

        printf("\n======%d=========\n==========\n%s\n===============\n",os.str().length(),os.str().c_str());
/*       
        std::string identity = "1008_ident";
        r = base_storage::MemDicSerial::SetString(identity.c_str(),identity.length(),
                                                   idpIdentityContextDump,strlen(idpIdentityContextDump)+1);

        std::string session = "1008_session";
        r = base_storage::MemDicSerial::SetString(session.c_str(),session.length(),
                                                  idpSessionContextDump,strlen(idpSessionContextDump)+1);

        std::string provider = "1008_provide"; 
        r = base_storage::MemDicSerial::SetString(provider.c_str(),provider.length(),
                                                  serviceProviderId,strlen(serviceProviderId)+1);

        std::string samp = "1008_samp";
        r = base_storage::MemDicSerial::SetString(samp.c_str(),samp.length(),
                                               Response,strlen(Response)+1);*/
	/*char* getvalue = new char[4096];
        size_t len1 = 0;
        r = base_storage::MemDicSerial::GetString(idp_check_key.c_str(),idp_check_key.length(),
                                                  &getvalue,&len1);
        printf("json[%s]\n",getvalue);  */     
        
        
        }
/*
        time_t time2 = time(NULL);
        printf("===========time [%lld]\n============\n",time2 - time1);*/
        lasso_server_destroy(spContext);
	lasso_login_destroy(spLoginContext);

	//SP

	spContext = lasso_server_new_from_dump(serviceProviderContextDump);
	spLoginContext = lasso_login_new_from_dump(spContext, spLoginDump);//spLohinDump可以重新创建。
// 	rc = lasso_login_init_request(spLoginContext,
// 		responseQuery,
// 		LASSO_HTTP_METHOD_REDIRECT);
//
        printf("Response[%s]",Response);
	rc = lasso_login_init_request(spLoginContext,Response,LASSO_HTTP_METHOD_POST);
//	assert(spLoginContext->parent.msg_relayState!=NULL);
//	assert(g_strcmp0(spLoginContext->parent.msg_relayState, relayState)==0);
	assert(rc==0);

	rc = lasso_login_build_request_msg(spLoginContext);
	assert(rc==0);
	soapRequestMsg = LASSO_PROFILE(spLoginContext)->msg_body; 

        //printf("\nsoapRequestMsg[%s]\n",soapRequestMsg);

       
        requestType = lasso_profile_get_request_type_from_soap_msg(soapRequestMsg);
        printf("==================requestType[%d]========================",requestType);
        idpContext = lasso_server_new_from_dump(identityProviderContextDump);
        idpLoginContext = lasso_login_new(idpContext);
  
        rc = lasso_login_process_request_msg(idpLoginContext,soapRequestMsg);


        idpLoginContext->assertionArtifact;

        //printf("\nidpLoginContext->assertionArtiface[%s]\n",idpLoginContext->assertionArtifact);
   

	rc = lasso_profile_set_session_from_dump(LASSO_PROFILE(idpLoginContext),
		 idpSessionContextDump);
	assert(rc==0);
	rc = lasso_login_build_response_msg(idpLoginContext, serviceProviderId);

	assert(rc==0);
	soapResponseMsg = LASSO_PROFILE(idpLoginContext)->msg_body;
        //printf("\nsoapResponseMsg[%s]\n",soapResponseMsg);
        //printf("\nserviceProviderID[%s]\n",serviceProviderId);


       rc = lasso_login_process_response_msg(spLoginContext,soapResponseMsg);

       rc = lasso_login_accept_sso(spLoginContext);
      
       spIdentityContextDump = lasso_identity_dump(LASSO_PROFILE(spLoginContext)->identity);
       
       //printf("\nspIdentityContextDump[%s]\n",spIdentityContextDump);

       spSessionDump = lasso_session_dump(LASSO_PROFILE(spLoginContext)->session);

       
}
