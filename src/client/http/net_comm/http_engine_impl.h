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
}

namespace net_comm{

class HttpEngineImpl:public net_comm::HttpEngine{
public:
	HttpEngineImpl();
	virtual ~HttpEngineImpl();
	virtual HttpEngine::HttpReturnStatus SetOutputHandler(HttpOutPutHandler* phoh);

	virtual HttpEngine::HttpReturnStatus SetUser(const net_comm::Hid& hid);

	virtual const net_comm::Hid & GetUser() const;

	virtual HttpEngine::HttpReturnStatus SetRequestedResource(const std::string& resource);
                    
	virtual const std::string& GetRequestedResource() const; 

	virtual HttpEngine::HttpReturnStatus SetPassword(const std::string& password);

	virtual const std::string& GetPassword() const;

	virtual HttpEngine::HttpReturnStatus OnUsrLogin();



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

private:
	friend class HttpLoginTask;
	void InternalRequestLogin(const std::string& password);
	void InternalGetUserInfoSelf(const std::string& uid);
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
	 net_comm::Hid   hid_;
	 std::string     resource_;
	 std::string     password_;
	 State state_;
	 Error error_;
	 int   engine_entered_;
	 std::string requested_resource_;
	 scoped_ptr<std::stringstream> output_;
	 net_comm::HttpOutPutHandler* output_handler_;
	 net_comm::HttpSessionHandler* session_handler_;
	 scoped_ptr<HttpLoginTask>  login_task_;

	 
};

}
#endif
