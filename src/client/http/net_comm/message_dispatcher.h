#ifndef _MESSAGE_DISPATCHER_H_
#define _MESSAGE_DISPATCHER_H_

////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <map>
#include "base/callback.h"
#include "base/lock.h"
#include "net_comm/interface.h"
#include "net_comm/buffered_queue.h"

////////////////////////////////////////////////////////////////////////////////
namespace net_comm {

class MessageDispatcher
{
public:
	MessageDispatcher(void);
	virtual ~MessageDispatcher(void);

	template <class T>
	void RegisterMessageHandler(int message_id, T &object, void (T::* method)(message::Message *)) {
		message_handlers_map_[message_id] = NewCallback(&object, method);
	}

	NET_COMMON_API void UnregisterMessageHandler(int message_id);

	NET_COMMON_API void UnregisterAllMessageHandler();

	NET_COMMON_API void DoMessageDispatch();

	void PostMessage(message::Message *message);

	void ClearMessages();

protected:
	typedef Callback1<message::Message *>::Type MessageHandler;
	typedef std::map<int, MessageHandler *> MessageHandlerMap;

	MessageHandlerMap message_handlers_map_;

	util::BufferedQueue<message::Message *> message_queue_;
};

} // namespace net_comm

#endif // _MESSAGE_DISPATCHER_H_
