#include "net_comm/http_client.h"
#include "net_comm/http_engine.h"
#include "net_comm/http_constants.h"
#include "net_comm/http_base.h"
#include "net_comm/http_client_settings.h"
#include "net_comm/hid.h"
#include "net_comm/schema_output.h"
#include "sigslot/sigslot.h"
#include "googleurl/src/gurl.h"

namespace net_comm{

class HttpClient::Privates:
	public sigslot::has_slots<>,
	public HttpOutPutHandler{

public:
	Privates(HttpClient * client)
		:client_(client)
		,engine_(NULL)
		,proxy_port_(0)
		,proxy_host_(STR_EMPTY)
		,pre_engin_error_(net_comm::HttpEngine::ERROR_NONE){

	}

	HttpClient* const client_;

	scoped_ptr<net_comm::HttpEngine> engine_;
	std::string                      password_;
	std::string                      proxy_host_;
	int                              proxy_port_;

	net_comm::HttpEngine::Error      pre_engin_error_;

	sigslot::signal0<>               SignalLogin_;
	sigslot::signal0<>               SignalRequest_;

	void WriteOutput(const std::string& request, std::string& output,
					int32& code);

	void WriteOutput(const std::string& request, const std::string& post_content,
		             std::string& output, int& code);
	//Event

	void OnEventLogin();
	void OnEventRequest();
	void OnEventGetUserInfo();

};

HttpClient::HttpClient(){
	ds_.reset(new Privates(this));
}

HttpClient::~HttpClient(){

}

void 
HttpClient::Privates::WriteOutput(const std::string& request, std::string& output,
								  int32& code){
	scoped_ptr<net_comm::HttpBase>  http_base;
	net_comm::HttpBase* http = new net_comm::HttpBase;
	net_comm::SchemaOutPut schema_output;
	GURL url(request);
	http_base.reset(http);
	http_base->GetSchemaOutPut(url.spec(),schema_output);
	//code = schema_output.status();
	//output.assign(schema_output.content())
	output.assign(schema_output.content()->content());

}

void 
HttpClient::Privates::WriteOutput(const std::string& request, const std::string& post_content,
								  std::string& output, int32& code){
	  scoped_ptr<net_comm::HttpBase>  http_base;
	  net_comm::HttpBase* http = new net_comm::HttpBase;
	  net_comm::SchemaOutPut schema_output;
	  GURL url(request);
	  http_base.reset(http);
	  http_base->PostSchemaOutPut(url.spec(),post_content,schema_output);
	  //code = schema_output.status();
	  //output.assign(schema_output.content())
	  output.assign(schema_output.content()->content());
}


net_comm::HttpEngine::HttpReturnStatus
HttpClient::Init(const net_comm::HttpClientSettings &settings, const std::string &lang){
	ds_->engine_.reset(HttpEngine::Create());
	ds_->engine_->SetOutputHandler(ds_.get());
	if (!settings.resource().empty()){
		ds_->engine_->SetRequestedResource(settings.resource());
	}

	ds_->engine_->SetUser(net_comm::Hid(settings.user(),settings.resource(),settings.host()));
	ds_->engine_->SetPassword(settings.pass());


	ds_->password_ = settings.pass();
	ds_->proxy_host_ = settings.proxy_host();
	ds_->proxy_port_ = settings.proxy_port();

	//Event Login
	ds_->SignalLogin_.connect(ds_.get(),&Privates::OnEventLogin);
	//Event Request
	ds_->SignalRequest_.connect(ds_.get(),&Privates::OnEventRequest);

	//fix me 修改状态
	
	return HttpEngine::HTTP_RETURN_OK;
}

net_comm::HttpEngine::HttpReturnStatus 
HttpClient::LoginOn(void){
	//fix me  暂未使用线程池 线程池中 将其投递到队列
	ds_->SignalLogin_();
	return HttpEngine::HTTP_RETURN_OK;
}


void HttpClient::Privates::OnEventLogin(){
	engine_->OnUsrLogin();
}

void HttpClient::Privates::OnEventRequest(){

}

void HttpClient::Privates::OnEventGetUserInfo(){

}

}