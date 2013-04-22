#ifndef _HTTP_NET_COMM_HTTP_ENGINE_IMPL_H__
#define _HTTP_NET_COMM_HTTP_ENGINE_IMPL_H__

#include <sstream>
#include "base/scoped_ptr.h"
#include "net_comm/http_engine.h"
#include "net_comm/hid.h"
#include "xmpp/xmppstanzaparser.h"

namespace net_comm{
	class HttpEngine;
	class HttpOutPutHandler;
	class HttpSessionHandler;
	//class Hid;
	class HttpLogicTask;
}

namespace net_comm{

class HttpEngineImpl:public net_comm::HttpEngine{
public:
	HttpEngineImpl();
	virtual ~HttpEngineImpl();

	virtual Error GetError( int *subcode );

	virtual HttpEngine::HttpReturnStatus SetOutputHandler(HttpOutPutHandler* phoh);

	virtual HttpEngine::HttpReturnStatus SetUser(const net_comm::Hid& hid);

	virtual const net_comm::Hid & GetUser() const;

	virtual HttpEngine::HttpReturnStatus SetRequestedResource(const std::string& resource);
                    
	virtual const std::string& GetRequestedResource() const; 

	virtual HttpEngine::HttpReturnStatus SetPassword(const std::string& password);

	virtual const std::string& GetPassword() const;

	virtual HttpEngine::HttpReturnStatus SetContent(const std::string& content);
	
	virtual const std::string& GetContent() const;

	virtual HttpEngine::HttpReturnStatus OnUsrLogin();

	virtual const HttpEngine::HttpTypeStatus GetHttpType() const;
	
	virtual HttpEngine::HttpReturnStatus SetHttpType(HttpEngine::HttpTypeStatus type);

	class StanzaParseHandler :public base::XmppStanzaParseHandler{
	public:
		StanzaParseHandler(HttpEngineImpl* outer):outer_(outer){}
		virtual void StartStream(const base::XmlElement* pelStream)
		{ outer_->IncomingStart(pelStream);}
		virtual void Stanza(const base::XmlElement * pelStanza)
		{outer_->IncomingStanza(pelStanza);}
		virtual void EndStream()
		{outer_->IncomingEnd(false);}
		virtual void XmlError()
		{outer_->IncomingEnd(true);}
	private:
		HttpEngineImpl * const  outer_;
	};

	void SignalError(Error errorCode, int subCode);
	bool HasError() { return error_code_ != ERROR_NONE; }
	void RaiseReset() { raised_reset_ = true; }

private:
	friend class HttpLoginTask;
	friend class HttpLogicTask;

	std::stringstream &output() { return *output_; }
	std::stringstream &output_post() { return *output_post_; }

	void IncomingStanza(const base::XmlElement* pelStanza);
	void IncomingStart(const base::XmlElement* pelStanza);
	void IncomingEnd(bool isError);

 class EnterExit{
 public:
	 EnterExit(HttpEngineImpl* engine);
	 ~EnterExit();

 private:
	 HttpEngineImpl* engine_;
	 HttpEngine::State state_;
	 HttpEngine::Error error_;
 };

 friend class EnterExit;
 friend class StanzaParseHandler;
 private:

	 StanzaParseHandler stanzaParseHandler_;
	 base::XmppStanzaParser stanzaParser_;
	 bool raised_reset_;

	 net_comm::Hid   hid_;
	 std::string     resource_;
	 std::string     password_;
	 std::string     token_;

	 State state_;
	 HttpEngine::HttpTypeStatus type_;
	 Error error_code_;
	 int subcode_;
	 int   engine_entered_;
	 std::string requested_resource_;

	 scoped_ptr<std::stringstream> output_;
	 scoped_ptr<std::stringstream> output_post_;
	 std::string content_;

	 net_comm::HttpOutPutHandler* output_handler_;
	 net_comm::HttpSessionHandler* session_handler_;
	 
	 scoped_ptr<HttpLoginTask>  login_task_;
	 scoped_ptr<HttpLogicTask>  logic_task_;
};

}
#endif
