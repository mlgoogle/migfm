#ifndef _NET_COMM_HTTP_LOGIN_TASK_H__
#define _NET_COMM_HTTP_LOGIN_TASK_H__

#include <string>


namespace net_comm{


class HttpEngineImpl;

class HttpLoginTask{

public:
	HttpLoginTask(HttpEngineImpl* pctx);
	~HttpLoginTask(void);
	void IncomingStanza(bool isStart);
	bool IsDone()
	{ return state_ == LOGINSTATE_DONE; }
private:
	enum LoginTaskState {
		LOGINSTATE_INIT = 0,
		LOGINSTATE_STREAMSTART_SENT,
		LOGINSTATE_STARTED_XMPP,
		LOGINSTATE_TLS_INIT,
		LOGINSTATE_AUTH_INIT,
		LOGINSTATE_BIND_INIT,
		LOGINSTATE_TLS_REQUESTED,
		LOGINSTATE_SASL_RUNNING,
		LOGINSTATE_BIND_REQUESTED,
		LOGINSTATE_SESSION_REQUESTED,
		LOGINSTATE_DONE,
	};

	bool Advance();

	LoginTaskState       state_;
	HttpEngineImpl*      pctx_;
	std::string          password_;
};

}
#endif