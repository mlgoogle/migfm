#ifndef _HTTP_NET_COMM_WRAPPER_HTTP_H__
#define _HTTP_NET_COMM_WRAPPER_HTTP_H__

//#include "googleurl/src/gurl.h"
#include "net_comm/export.h"
#include "base/scoped_ptr.h"
#include "base/logging.h"
#include "net_comm/interface.h"
#include "net_comm/schema_output.h"
#include "net_comm/http_base.h"
#include "net_comm/hid.h"
#include "net_comm/http_client_settings.h"

namespace net_comm{
	class HttpClient;
}

namespace net_comm{

class PE_EXPORT WrapperHttp{
public:
	WrapperHttp();
	~WrapperHttp();
	static WrapperHttp* GetInstance(void);
	static void FreeInstance(void);
	void Init(std::string& resource,std::string& domain,bool tls = false);
	bool UserLogin(std::string &usrname, std::string &passwd,std::string& hostname);

private:
	static WrapperHttp*             instance_;
	scoped_ptr<net_comm::HttpBase>  http_base_;
	net_comm::Hid                   hid_;
	net_comm::HttpClientSettings    xcs_;
	scoped_ptr<net_comm::HttpClient> http_client_;
};

}

#endif