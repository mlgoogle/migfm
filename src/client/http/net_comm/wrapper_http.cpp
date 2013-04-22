#include "net_comm/wrapper_http.h"
#include "net_comm/http_client.h"
#include "googleurl/src/gurl.h"

namespace net_comm{

WrapperHttp* WrapperHttp::instance_ = NULL;

WrapperHttp::WrapperHttp(){
	http_client_.reset(new net_comm::HttpClient);
	http_base_.reset(new net_comm::HttpBase);
}

WrapperHttp::~WrapperHttp(){

}

void WrapperHttp::Init(MigHttpModuleImpl *module, const std::string& resource, const std::string& domain, bool tls){

	http_module_ = module;
	xcs_.set_resource(resource);
	xcs_.set_host(domain);
	xcs_.set_use_tls(tls);
}


bool WrapperHttp::UserLogin(const std::string &usrname, const std::string &passwd, const std::string& hostname){
	std::string  lang;
	hid_ = net_comm::Hid(usrname,hostname);
	if (!hid_.IsValid()){
		return false;
	}
	xcs_.set_user(hid_.username());
	xcs_.set_pass(CryptString(passwd));

	//Init UserInfo
	http_client_.get()->Init(xcs_,lang);

	http_client_.get()->LoginOn();

	return true;
}

WrapperHttp* WrapperHttp::GetInstance(){
	if (instance_==NULL){
		instance_ = new WrapperHttp();
	}
	return instance_;
}

void WrapperHttp::FreeInstance(){
	delete instance_;
}

}