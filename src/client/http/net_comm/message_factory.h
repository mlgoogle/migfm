#ifndef _MESSAGE_FACTORY_H_
#define _MESSAGE_FACTORY_H_

#pragma once
#include "interface.h"
#include "base/singleton.h"
#include "message.h"

namespace net_comm {
namespace message {

#define MessageFactoryInstance() MessageFactory::GetInstance()

class Message;

class NET_COMMON_API MessageFactory
{
public:
	static MessageFactory *GetInstance();

public:
	template <class T>
	T *CreateMessage() {
		return new T();
	}

	void FreeMessage(Message *message) {
		delete message;
	}

public:
	virtual ~MessageFactory(void);
private:
	MessageFactory(void);
	DISALLOW_COPY_AND_ASSIGN(MessageFactory);

	friend class Singleton<MessageFactory>;
	friend struct DefaultSingletonTraits<MessageFactory>;
};

} // namespace message
} // namespace net_comm

#endif // _MESSAGE_FACTORY_H_
