/*******************************************************************************
	created:	2013/03/18
	created:	18:3:2013   22:47
	filename: 	client\http\net_comm\mig_http_module_impl.cc
	author:		huaiyu
	
	purpose:	
*******************************************************************************/
#include "mig_http_module_impl.h"
#include "base\logging.h"
#include "message_dispatcher.h"
#include "message.h"
#include "wrapper_http.h"
#include "message_factory.h"

////////////////////////////////////////////////////////////////////////////////
namespace {

class QuitThreadMessage : public net_comm::message::Message {
public:
	QuitThreadMessage() : Message(net_comm::MID_I_QUIT) {}
};

} // namespace

////////////////////////////////////////////////////////////////////////////////
namespace net_comm {

//******************************************************************************
MigHttpModuleImpl::MigHttpModuleImpl(void)
	: SimpleThread("net_http")
	, message_dispatcher_(new MessageDispatcher())
	, request_complete_cb_(NULL)
	, message_queue_event_(false, false)
	, run_message_loop_(false)
{
}

MigHttpModuleImpl::~MigHttpModuleImpl(void)
{
	QuitWorkThread();
}

void MigHttpModuleImpl::Run()
{
	while (1) {
		// 等待请求消息
		if (!message_queue_event_.Wait())
			break;

		// 分发请求消息
		if (!DispatchMessage())
			break;
	}
}

bool MigHttpModuleImpl::DispatchMessage()
{
	while (message_queue_.Reload()) {
		message::Message *message = message_queue_.Pop();
		DCHECK(message);

		if (MID_I_QUIT == message->id()) {
			delete (message);
			// 清理未处理的消息
			ClearMessagesQueue();
			return false;
		}

		HandleMessage(message);
		delete (message);
		if (request_complete_cb_) {
			request_complete_cb_->OnHttpRequestCompleted();
		}
	}

	return true;
}

void MigHttpModuleImpl::HandleMessage( message::Message *message )
{
	using namespace message;

	switch (message->id()) {
	case MID_HTTP_GET: {
		DoHttpGet(message);
		break;
	}case MID_HTTP_POST: {
		DoHttpPost(message);
		break;
	}
	case MID_LOGIN: {
		DoLogin(message);
		break;
	}
	default:
		break;
	}
}

void MigHttpModuleImpl::Release()
{
	delete this;
}

bool MigHttpModuleImpl::Init( const std::string& host, const std::string& entry,
							 MigHttpModuleCallbackInterface *cb, const void *reserved/*=NULL */ )
{
	assert(cb);

	request_complete_cb_ = cb;

	net_comm::WrapperHttp::GetInstance()->Init(this, entry,host);

	// 开启工作线程
	Start();
	run_message_loop_ = true;
	return true;
}

void MigHttpModuleImpl::Uninit()
{
	QuitWorkThread();

	net_comm::WrapperHttp::FreeInstance();
}

void MigHttpModuleImpl::PushMessage( message::Message *message )
{
	DCHECK(message);
	DCHECK(message->id() >= 0);
	if (!run_message_loop_) {
		return;
	}

	InternalPushMessage(message);
}

void MigHttpModuleImpl::InternalPushMessage( message::Message *message )
{
	message_queue_.Push(message);
	message_queue_event_.Signal();
}

MessageDispatcher * MigHttpModuleImpl::GetMessageDispatcher()
{
	return message_dispatcher_.get();
}

void MigHttpModuleImpl::QuitWorkThread()
{
	InternalPushMessage(new QuitThreadMessage());
	run_message_loop_ = false;
	Join();

	ClearMessagesQueue();
}

void MigHttpModuleImpl::ClearMessagesQueue()
{
	while (message_queue_.Reload()) {
		delete (message_queue_.Pop());
	}
}

void MigHttpModuleImpl::DoHttpGet( message::Message *message )
{
	using namespace message;

	const MessageHttpGet_Launch *launch = static_cast<const MessageHttpGet_Launch *>(message);
	MessageHttpGet_Result result;
	result.url = launch->url;
	result.result = "OK";
	::Sleep(3000);
	message_dispatcher_->PostMessage(&result);
}

void MigHttpModuleImpl::DoHttpPost( message::Message *message )
{

}

void MigHttpModuleImpl::DoLogin( message::Message *message )
{
	using namespace message;

	const MessageLogin_Launch *login = static_cast<const MessageLogin_Launch *>(message);
	net_comm::WrapperHttp::GetInstance()->UserLogin(
		login->name, login->password, login->host);
}

} // namespace net_comm