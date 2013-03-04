#ifndef _NET_COMM_REPONSE_H__
#define _NET_COMM_REPONSE_H__

#include <vector>

class GURL;

namespace net{
	namespace type{
		class Metadatas;
	}
}

namespace net_comm{

class Response{
public:
	Response(void){}
	virtual ~Response(void){}
	typedef std::vector<char> ContentType;
	//Get the responese frome server
	virtual bool Get(void) = 0;

	//Post to the server
	virtual bool Post(const std::string& post_content) = 0;

	//Returns the URL used to retrieve this response.
	virtual const GURL& GetUrl(void) = 0;

	// Returns the response code.
	virtual int GetCode(void) = 0;

	// Returns the value of a named header.
	virtual const std::string& GetHeader(std::string& name) = 0;
   
	// Returns all the header.
	virtual net::type::Metadatas *GetHeaders(void) = 0;

	// Returns the full content of the response.
	virtual const ContentType& GetContent(void) = 0;


};


}

#endif