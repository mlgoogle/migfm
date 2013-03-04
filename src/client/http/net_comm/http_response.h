#ifndef _NET_COMM_HTTP_RESPONSE_H__
#define _NET_COMM_HTTP_RESPONSE_H__

#include "net_comm/response.h"

namespace net_comm{
	class HttpBase;
}

class GURL;

namespace net{
	namespace type{
		class Metadatas;
	}
}

namespace net_comm{

class HttpResponse: public net_comm::Response{
public:
	explicit HttpResponse(HttpBase* http,const GURL& url);

	virtual ~HttpResponse(void);

	// Get the response from server.
	virtual bool Get(void);

	//Post to the server
	virtual bool Post(const std::string& post_content);

	virtual const GURL& GetUrl(void) {return url_;}

	virtual int GetCode(void) {return code_;}

	virtual const std::string& GetHeader(std::string& name);

	virtual net::type::Metadatas *GetHeaders(void){return headers_;}

	virtual const ContentType& GetContent(void) {return content_;}

private:
	HttpBase *http_;
	const GURL& url_;
	ContentType content_;
	int code_;
	net::type::Metadatas *headers_;
};

}

#endif