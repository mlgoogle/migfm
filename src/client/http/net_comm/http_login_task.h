#ifndef _NET_COMM_HTTP_LOGIN_TASK_H__
#define _NET_COMM_HTTP_LOGIN_TASK_H__

#include <string>
#include "http_task_base.h"

namespace net_comm{

class HttpEngineImpl;

class HttpLoginTask : public HttpTaskBase {

public:
	HttpLoginTask(HttpEngineImpl* pctx);
	virtual ~HttpLoginTask(void);

	virtual void IncomingStanza(const base::XmlElement * element, bool isStart);

	virtual bool IsDone()
	{ return state_ == LOGINSTATE_DONE; }

	void SetStateLogin()
	{ state_ = LOGINSTATE_INIT; }

private:
	enum LoginTaskState {
		LOGINSTATE_INIT = 0,
		LOGINSTATE_IDP,
		LOGINSTATE_TOKEN,
// 		LOGINSTATE_TLS_INIT,
// 		LOGINSTATE_AUTH_INIT,
// 		LOGINSTATE_BIND_INIT,
// 		LOGINSTATE_TLS_REQUESTED,
// 		LOGINSTATE_SASL_RUNNING,
// 		LOGINSTATE_BIND_REQUESTED,
// 		LOGINSTATE_SESSION_REQUESTED,
		LOGINSTATE_DONE,
	};

	bool Advance();
	void HttpLoginTask::InternalLoginInit(const std::string& password);
	void HttpLoginTask::InternalRequestIdp(const std::string& content);
	void HttpLoginTask::InternalRequestToken( const std::string& content );
	void HttpLoginTask::InternalTokenResult( const std::string& content );
	LoginTaskState       state_;
	HttpEngineImpl*      pctx_;
	std::string          password_;
};

}
#endif