#ifndef _NET_COMM_HTTP_ENGINE_H__
#define _NET_COMM_HTTP_ENGINE_H__
#include <string>
namespace net_comm{
	class Hid;
}


namespace net_comm{



class HttpOutPutHandler {
public:
	virtual void WriteOutput(std::string& request,std::string& output,
		                     int& code) = 0;

	virtual void WriteOutput(std::string& request,std::string& post_content,
							std::string& output,int& code) = 0; 
	//virtual void StartTls(const std::string& domain) = 0;

	//virtual void CloseConnection() = 0;
};

class HttpSessionHandler {
public:
	virtual void OnStateChange(int state) = 0;
};


class HttpEngine{
public:
	enum Error{
		ERROR_NONE = 0
	};

	enum State{
		STATE_NONE = 0,
		STATE_START,
		STATE_OPENING, 
		STATE_OPEN,
		STATE_CLOSED
	};	

	enum HttpReturnStatus {
		HTTP_RETURN_OK,
		HTTP_RETURN_BADARGUMENT,
		HTTP_RETURN_BADSTATE,
		HTTP_RETURN_PENDING,
		HTTP_RETURN_UNEXPECTED,
		HTTP_RETURN_NOTYETIMPLEMENTED
	};

	//HttpEngine(void);
	static HttpEngine* Create();
	~HttpEngine(void){};

	virtual HttpReturnStatus SetOutputHandler(HttpOutPutHandler* phoh) = 0;


	virtual HttpReturnStatus SetUser(const net_comm::Hid& hid) = 0;

	virtual const net_comm::Hid& GetUser() const = 0;

	virtual HttpReturnStatus SetRequestedResource(const std::string& resource) = 0;

	virtual const std::string& GetRequestedResource() const = 0;

	virtual HttpEngine::HttpReturnStatus SetPassword(const std::string& password)= 0;

	virtual const std::string& GetPassword() const = 0;


	virtual HttpReturnStatus OnUsrLogin() = 0;

};
}
#endif