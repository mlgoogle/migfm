#ifndef _NET_COMM_HTTP_CLIENT_H__
#define _NET_COMM_HTTP_CLIENT_H__

#include "net_comm/http_engine.h"
#include <string>
#include "base/scoped_ptr.h"


namespace net_comm{
	class HttpClientSettings;
}

namespace net_comm{

class HttpClient{
public:
	HttpClient(void);
	~HttpClient();
	net_comm::HttpEngine::HttpReturnStatus Init(const HttpClientSettings& settings,
			 const std::string& lang);

	net_comm::HttpEngine::HttpReturnStatus LoginOn(void);

	class Privates;
	friend class Privates;
	scoped_ptr<Privates>  ds_;
};

}
#endif