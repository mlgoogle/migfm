#include "http_engine_impl.h"

#include <map>
#include <string.h>

#include "googleurl/src/gurl.h"
#include "base/string_util.h"

#include "net_comm/http_engine.h"
#include "net_comm/http_constants.h"
#include "net_comm/xmpp/xmlelement.h"
#include "net_comm/xmpp/constants.h"
#include "net_comm/http_login_task.h"
#include "net_comm/http_logic_task.h"

namespace net_comm{

HttpEngine* HttpEngine::Create(){
	return  new HttpEngineImpl;
}

HttpEngineImpl::HttpEngineImpl()
	: hid_(HID_EMPTY)
	, stanzaParseHandler_(this)
	, stanzaParser_(&stanzaParseHandler_)
	, raised_reset_(false)
	, resource_()
	, requested_resource_()
	, output_(new std::stringstream())
	, output_post_(new std::stringstream())
	, output_handler_(NULL)
	, session_handler_(NULL)
	, state_(STATE_START)
	, error_code_(ERROR_NONE)
	, subcode_(0)
	, engine_entered_(0) {
  
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
,error_(engine->error_code_){
	++engine->engine_entered_;
}

HttpEngineImpl::EnterExit::~EnterExit(){
	HttpEngineImpl* engine = engine_;
	--engine_->engine_entered_;
	
	bool closing = (engine->state_!=state_ && engine->state_==STATE_CLOSED);
	bool flushing = closing || (engine->engine_entered_ == 0);

	if (engine->output_handler_ && flushing) {
		if (STATE_LOGIC == engine->state_)
			(*engine->output_) << "&token=" << engine->token_;

		std::string output = engine->output_->str();
		std::string result;
		int32 code = 0;
		if (output.length() > 0) {
			if (engine_->GetHttpType()==HttpEngineImpl::HTTP_GET_TYPE) {
				// for GET
				engine_->output_handler_->WriteOutput(output, result, code);
			} else if (engine_->GetHttpType()==HttpEngineImpl::HTTP_POST_TYPE){
				// for POST
				std::string output_post = engine_->output_post_->str();
				engine_->output_handler_->WriteOutput(output, output_post, result,code);
			}
		}
		engine->output_->str("");
		engine->output_post_->str("");
		engine->SetContent(result);

		if (STATE_LOGIC == engine->state_) {
			engine->stanzaParser_.Reset();
			engine->stanzaParser_.Parse(result.c_str(), result.length(), true);
		}

		if (closing) {
			//engine->output_handler_->CloseConnection();
			engine->output_handler_ = 0;
		}
	}

	if (engine->engine_entered_)
		return;

	if (engine->raised_reset_) {
		engine->stanzaParser_.Reset();
		engine->raised_reset_ = false;
	}

	if (engine->session_handler_) {
		if (engine->state_ != state_)
			engine->session_handler_->OnStateChange(engine->state_);
		// Note: Handling of OnStateChange(CLOSED) should allow for the
		// deletion of the engine, so no members should be accessed
		// after this line.
	}
	return;

	std::string content;
	int32 code = 0;
	std::string url = engine_->output_->str();
	if (url.length()>0&&engine_->output_handler_) {
		switch (state_){
			case STATE_LOGIN: {
				if (engine_->GetHttpType()==HttpEngineImpl::HTTP_GET_TYPE)
					engine_->output_handler_->WriteOutput(url,content,code);
				else if (engine_->GetHttpType()==HttpEngineImpl::HTTP_POST_TYPE){
					std::string post_content = engine_->GetContent();
					engine_->output_handler_->WriteOutput(url,post_content,content,code);
				}
				engine->SetContent(content);
				engine->output_->str("");
				engine_->login_task_->IncomingStanza(NULL, false);
				break;
			}
			case STATE_LOGIC: {
				break;
			}
		}
	}
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
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetOutputHandler(net_comm::HttpOutPutHandler *phoh){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADSTATE;
	output_handler_ = phoh;
	return HTTP_RETURN_OK;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetUser(const net_comm::Hid &hid){
	if(state_!=STATE_START)
		return HTTP_RETURN_BADSTATE;
	hid_ = hid;
	return HTTP_RETURN_OK;
}

const net_comm::Hid& HttpEngineImpl::GetUser() const{
	return hid_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetPassword(const std::string& password){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADSTATE;
	password_ = password;
	return HTTP_RETURN_OK;
}

const std::string& HttpEngineImpl::GetPassword() const{
	return password_;
}

const std::string& HttpEngineImpl::GetContent() const{
	return content_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetRequestedResource(const std::string &resource){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADSTATE;
	resource_ = resource;
	return HTTP_RETURN_OK;
}

const std::string& HttpEngineImpl::GetRequestedResource() const{

	return resource_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::OnUsrLogin(){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADSTATE;

	state_ = STATE_LOGIN;
	if (login_task_.get()) {
		login_task_->IncomingStanza(NULL, false);
		if (login_task_->IsDone()) {
			login_task_.reset();

			state_ = STATE_LOGIC;
			logic_task_.reset(new HttpLogicTask(this));

			logic_task_->GetUserInfo(hid_.username());
		}
	}

	return HTTP_RETURN_OK;
}

void 
HttpEngineImpl::IncomingStanza(const base::XmlElement *pelStanza){
	if (HasError() || raised_reset_)
		return;

	if (pelStanza->Name()==base::QN_STREAM_FEATURES)
		return ;

	if (logic_task_.get())
		logic_task_->IncomingStanza(pelStanza, false);
}

void
HttpEngineImpl::IncomingStart(const base::XmlElement* pelStanza){
	if (HasError() || raised_reset_)
		return;
}

void HttpEngineImpl::IncomingEnd(bool isError){
	if (HasError() || raised_reset_)
		return;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetContent( const std::string& content )
{
	content_ = content;
	return HTTP_RETURN_OK;
}

const HttpEngine::HttpTypeStatus HttpEngineImpl::GetHttpType() const{
	return type_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetHttpType(HttpEngine::HttpTypeStatus type){
	type_ = type;
	return HTTP_RETURN_OK;
}

void HttpEngineImpl::SignalError( Error errorCode, int subCode ) {
	if (state_ != STATE_CLOSED) {
		error_code_ = errorCode;
		subcode_ = subCode;
		state_ = STATE_CLOSED;
	}
}

HttpEngineImpl::Error HttpEngineImpl::GetError( int *subcode ) {
	if (subcode) {
		*subcode = subcode_;
	}
	return error_code_;
}

}