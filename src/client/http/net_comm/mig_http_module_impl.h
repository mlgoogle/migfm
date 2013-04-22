/*******************************************************************************
	created:	2013/03/18
	created:	18:3:2013   22:44
	filename: 	client\http\net_comm\mig_http_module_impl.h
	author:		huaiyu
	
	purpose:	
*******************************************************************************/
#ifndef _NET_COMM_MIG_HTTP_MODULE_IMPL_H_
#define _NET_COMM_MIG_HTTP_MODULE_IMPL_H_

#pragma once
#include "interface.h"
#include "base/scoped_ptr.h"
#include "sigslot/sigslot.h"
#include "base/simple_thread.h"
#include "base/lock.h"
#include "buffered_queue.h"

////////////////////////////////////////////////////////////////////////////////
namespace base {
	class SimpleThread;
} // namespace base

////////////////////////////////////////////////////////////////////////////////
namespace net_comm {

class Message;
class MigHttpModuleCallbackInterface;

enum MessageIDInternal {
	MID_I_QUIT = -1,
};

class MigHttpModuleImpl :
	public base::SimpleThread,
	public MigHttpModuleInterface
{
public:
	MigHttpModuleImpl(void);
	virtual ~MigHttpModuleImpl(void);

	virtual void Release();

	virtual bool Init( const std::string& host, const std::string& entry,
					   MigHttpModuleCallbackInterface *cb, const void *reserved=NULL );

	virtual void Uninit();

	virtual void PushMessage( message::Message *message );

	virtual MessageDispatcher * GetMessageDispatcher();

	virtual void Run();

protected:
	void InternalPushMessage(message::Message *message);

	bool DispatchMessage();

	void HandleMessage(message::Message *message);

	void QuitWorkThread();

	void ClearMessagesQueue();

private:
	void DoHttpGet(message::Message *message);
	void DoHttpPost(message::Message *message);
	void DoLogin(message::Message *message);

protected:
	scoped_ptr<MessageDispatcher> message_dispatcher_;
	MigHttpModuleCallbackInterface *request_complete_cb_;
;
	bool run_message_loop_;
	util::BufferedQueue<message::Message *> message_queue_;
	base::WaitableEvent message_queue_event_;

	friend class WrapperHttp;
};

} // namespace net_comm

#endif // _NET_COMM_MIG_HTTP_MODULE_IMPL_H_
