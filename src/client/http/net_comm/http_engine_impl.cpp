#include "http_engine_impl.h"
#include "net_comm/http_engine.h"
#include "net_comm/http_constants.h"
#include "net_comm/httplogintask.h"
#include "net_comm/xmpp/xmlelement.h"
#include "net_comm/xmpp/constants.h"
#include "googleurl/src/gurl.h"

namespace net_comm{

HttpEngine* HttpEngine::Create(){
	return  new HttpEngineImpl;
}

HttpEngineImpl::HttpEngineImpl()
:hid_(HID_EMPTY)
,stanzaParseHandler_(this)
,stanzaParser_(&stanzaParseHandler_)
,resource_()
,requested_resource_()
,output_(new std::stringstream())
,output_handler_(NULL)
,session_handler_(NULL)
,state_(STATE_START)
,error_(ERROR_NONE){
  
	login_task_.reset(new HttpLoginTask(this));
	std::string head;


	head = "<stream:stream from=\"gmail.com\" " 
		"id=\"1F83A90940271513\" "  //通过SSO获取 
		"version=\"1.0\" " 
		"xmlns:stream=\"http://etherx.jabber.org/streams\" " 
		"xmlns=\"jabber:client\">";

	stanzaParser_.Parse(head.c_str(),head.length(),false);
}

HttpEngineImpl::~HttpEngineImpl(){

}


HttpEngineImpl::EnterExit::EnterExit(net_comm::HttpEngineImpl *engine)
:engine_(engine)
,state_(engine->state_)
,error_(engine->error_){
	engine->engine_entered_+=1;
}

HttpEngineImpl::EnterExit::~EnterExit(){
	HttpEngineImpl* engine = engine_;
	engine_->engine_entered_-=1;
	int closeing = 0;
	std::string content;
	int32 code;
	std::string url = engine_->output_->str();
    if (url.length()>0&&engine_->output_handler_)
		engine_->output_handler_->WriteOutput(url,content,code);
	///////////////////////////////test////////////////////////////////////
 
// 
// 	std::string content;
// 	content = "<stream:features>"
// 				"<starttls xmlns=\"urn:ietf:params:xml:ns:xmpp-tls\">"
// 				"<required/>"
// 				"</starttls>"
// 				"<mechanisms xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
// 				"<mechanism>X-GOOGLE-TOKEN</mechanism><"
// 				"mechanism>X-OAUTH2</mechanism>"
// 				"</mechanisms>"
// 			"</stream:features>";
// 
// 	//stanzaParser_.Parse(content.c_str(),content.length(),false);
 	engine_->stanzaParser_.Parse(content.c_str(),content.length(),false);
// 	engine_->output_->str("");
	if (closeing){
		//engine_->output_handler_->CloseConnection();
		engine_->output_handler_ = NULL;
	}

	if (engine_->engine_entered_)
		return;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetOutputHandler(net_comm::HttpOutPutHandler *phoh){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	output_handler_ = phoh;
	return HTTP_RETURN_OK;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetUser(const net_comm::Hid &hid){
	if(state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	hid_ = hid;
	return HTTP_RETURN_OK;
}

const net_comm::Hid& HttpEngineImpl::GetUser() const{
	return hid_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetPassword(const std::string& password){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	password_ = password;
	return HTTP_RETURN_OK;
}

const std::string& HttpEngineImpl::GetPassword() const{
	return password_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetRequestedResource(const std::string &resource){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	resource_ = resource;
	return HTTP_RETURN_OK;
}

const std::string& HttpEngineImpl::GetRequestedResource() const{

	return resource_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::OnUsrLogin(){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	EnterExit ee(this);
	state_ = STATE_OPENING;
	if (login_task_.get()){
		login_task_->IncomingStanza(false);
		if (login_task_->IsDone())
			login_task_.reset();
	}

	return HTTP_RETURN_OK;
}

void 
HttpEngineImpl::InternalRequestLogin(const std::string& password){
	//*output_<<"http://www.9see.com/ashx/getlist.ashx";
	*output_<<"http://42.121.112.248/cgi-bin/show.fcgi?userid=10001";
}


void 
HttpEngineImpl::InternalGetUserInfoSelf(const std::string& uid){
	*output_<<"http://42.121.112.248/cgi-bin/show.fcgi?userid="<<uid<<";";
}

void 
HttpEngineImpl::IncomingStanza(const base::XmlElement *pelStanza){
	if (pelStanza->Name()==base::QN_STREAM_FEATURES)
		return ;
}

void
HttpEngineImpl::IncomingStart(const base::XmlElement* pelStanza){

}

void HttpEngineImpl::IncomingEnd(bool isError){

}

}