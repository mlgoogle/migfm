#ifndef _HTTP_LOGIC_TASK_H_
#define _HTTP_LOGIC_TASK_H_

#pragma once
#include "http_task_base.h"
#include <string>

namespace net_comm {

class HttpEngineImpl;

class HttpLogicTask : public HttpTaskBase
{
public:
	HttpLogicTask(HttpEngineImpl* pctx);
	virtual ~HttpLogicTask(void);

	virtual void IncomingStanza(const base::XmlElement * element, bool isStart);

	virtual bool IsDone() { return LOGICSTATE_DONE == state_; }

public:
	void GetUserInfo(const std::string &user_name);

private:
	void OnResultGetUserInfo(const base::XmlElement *xml);

private:
	void Advance();

private:
	enum LogicTaskState {
		LOGICSTATE_INIT = 0,
		LOGICSTATE_GET_USER_INFO,
		LOGICSTATE_DONE,
	};

private:
	HttpEngineImpl* pctx_;
	LogicTaskState state_;

	const base::XmlElement *pelStanza_;
	bool isStart_;
};

} // namespace net_comm

#endif // _HTTP_LOGIC_TASK_H_
