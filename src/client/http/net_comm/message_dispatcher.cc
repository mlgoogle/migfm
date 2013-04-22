#include "message_dispatcher.h"
#include "base\logging.h"
#include "message.h"
#include "message_factory.h"

namespace net_comm {

MessageDispatcher::MessageDispatcher(void)
{
}

MessageDispatcher::~MessageDispatcher(void)
{
	ClearMessages();
	UnregisterAllMessageHandler();
}

void MessageDispatcher::UnregisterMessageHandler( int message_id )
{
	MessageHandlerMap::iterator it = message_handlers_map_.find(message_id);
	if (it != message_handlers_map_.end()) {
		delete it->second;
		message_handlers_map_.erase(it);
	}
}

void MessageDispatcher::UnregisterAllMessageHandler()
{
	for (MessageHandlerMap::iterator it = message_handlers_map_.begin();
		it != message_handlers_map_.end();
		++it) {
		delete it->second;
	}
	message_handlers_map_.clear();
}

void MessageDispatcher::DoMessageDispatch()
{
	while (message_queue_.Reload()) {
		message::Message *message = message_queue_.Pop();
		DCHECK(message);

		MessageHandlerMap::iterator it = message_handlers_map_.find(message->id());
		if (it != message_handlers_map_.end()) {
			MessageHandler *handler = it->second;
			handler->Run(message);
		}
		delete (message);
	}
}

void MessageDispatcher::PostMessage( message::Message *message )
{
	message_queue_.Push(message);
}

void MessageDispatcher::ClearMessages()
{
	while (message_queue_.Reload()) {
		delete (message_queue_.Pop());
	}
}

} // namespace net_comm