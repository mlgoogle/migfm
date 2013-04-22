#include "http_login_task.h"
#include <string>
#include <iostream>

#include "net_comm/http_engine_impl.h"

namespace net_comm {

HttpLoginTask::HttpLoginTask(HttpEngineImpl* pctx)
	: pctx_(pctx)
	, state_(LOGINSTATE_INIT){

}

HttpLoginTask::~HttpLoginTask(){

}

void
HttpLoginTask::IncomingStanza(const base::XmlElement * element, bool isStart){
	Advance();
}

bool
HttpLoginTask::Advance() {
	for ( ; ; ) {
		switch (state_) {
		case  LOGINSTATE_INIT:
			InternalLoginInit(pctx_->GetPassword());
			state_ = LOGINSTATE_IDP;
			continue;
		case LOGINSTATE_IDP:
			InternalRequestIdp(pctx_->GetContent());
			state_ = LOGINSTATE_TOKEN;
			continue;
		case LOGINSTATE_TOKEN:
			InternalRequestToken(pctx_->GetContent());
			InternalTokenResult(pctx_->GetContent());
			state_ = LOGINSTATE_DONE;
			return true;
		}
	}
}

void 
HttpLoginTask::InternalLoginInit(const std::string& password){
	//*output_<<"http://www.9see.com/ashx/getlist.ashx";
	//*output_<<"http://42.121.112.248/cgi-bin/show.fcgi?userid=10001";
	//*output_<<"http://60.191.220.135/cgi-bin/echo1.fcgi";

	HttpEngineImpl::EnterExit ee(pctx_);

	std::stringstream &output = pctx_->output();

	pctx_->SetHttpType(HttpEngineImpl::HTTP_GET_TYPE);
	output << "http://sso.miglab.com/cgi-bin/sp.fcgi?sp";
}

void
HttpLoginTask::InternalRequestIdp(const std::string& content) {
	HttpEngineImpl::EnterExit ee(pctx_);

	//并将用户名与密码和sp返回信息拼接在一起 提交于idp 验证中心
	std::stringstream &output = pctx_->output();
	std::stringstream &output_post = pctx_->output_post();

	std::string::size_type pos = content.find("?");
	std::string post_url = content.substr(0,pos);
	std::string post_content = content.substr(pos+1,content.length());

	pctx_->SetHttpType(HttpEngineImpl::HTTP_POST_TYPE);

	output << post_url.c_str();

	output_post << "username="
		<< pctx_->GetUser().username()
		<< "&password="
		<< pctx_->GetPassword()
		<< "&"
		<< post_content;
}

void HttpLoginTask::InternalRequestToken( const std::string& content )
{
	using namespace std;

	HttpEngineImpl::EnterExit ee(pctx_);

	// content 内容如下：
	//content = "http://fm.miglab.com/cgi-bin/sp.fcgi?SAMLart=AAOfv3WG35avZspzKhoeodwv2MFd8zkyRjUwRDIzQjI2QThCNzVGMzEx&RelayState=fake%5B%5D";
	//	如果验证成功 则idp 将会返回ticket。ticket和ticket验证中心地址解析出来， 并提交ticket验证中心
	std::stringstream &output = pctx_->output();
	std::stringstream &output_post = pctx_->output_post();

	std::string post_content;
	std::string::size_type pos = content.find("?");
	std::string ticket_url = content.substr(0,pos);
	std::string ticket_content = content.substr(pos+1,content.length());

	pctx_->SetHttpType(HttpEngineImpl::HTTP_POST_TYPE);

	output << ticket_url;
	output_post << ticket_content;
}

void HttpLoginTask::InternalTokenResult( const std::string& content )
{
	using namespace std;

	pctx_->token_ = content;

	//------------------------------------------------------------------------------
	// FIXME: 
// 	string url, result;
// 	int32 code = 0;
// 	url = "http://42.121.112.248/cgi-bin/show.fcgi?";
// 	url += "username=";
// 	url += GetUser().username();
// 	url += "&token=";
// 	url += token_;
// 	output_handler_->WriteOutput(url, result, code);
// 
// 	stanzaParser_.Reset();
// 	stanzaParser_.Parse(result.c_str(),result.length(),true);
// 
// 	bool b = IsStringUTF8(result);
}

}
