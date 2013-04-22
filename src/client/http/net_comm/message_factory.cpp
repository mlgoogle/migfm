#include "message_factory.h"

namespace net_comm {
namespace message {

MessageFactory * MessageFactory::GetInstance()
{
	return Singleton<MessageFactory>::get();
}

MessageFactory::MessageFactory(void)
{
}

MessageFactory::~MessageFactory(void)
{
}

} // namespace message
} // namespace net_comm