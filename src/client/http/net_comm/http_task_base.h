#ifndef _HTTP_TASK_BASE_H_
#define _HTTP_TASK_BASE_H_

////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace base {
	class XmlElement;
}

namespace net_comm {

class HttpTaskBase
{
public:
	HttpTaskBase(void) {}
	virtual ~HttpTaskBase(void) {}

	virtual void IncomingStanza(const base::XmlElement * element, bool isStart) = 0;

	virtual bool IsDone() = 0;
};

} // namespace net_comm

#endif // _HTTP_TASK_BASE_H_
