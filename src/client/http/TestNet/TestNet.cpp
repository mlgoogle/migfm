// TestNet.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include "net_comm/interface.h"
#include "net_comm/message.h"
#include "net_comm/message_dispatcher.h"
#include "net_comm/message_factory.h"
#include <iostream>
 
namespace {
HANDLE g_event;
} // namespace

class Test : public net_comm::MigHttpModuleCallbackInterface {
public:
	virtual void OnHttpRequestCompleted() 
	{
		::SetEvent(g_event);
	}

	void OnMessageResult_HttpGet(net_comm::message::Message *message) {
		using namespace std;
		using namespace net_comm::message;

		const MessageHttpGet_Result *result = static_cast<const MessageHttpGet_Result *>(message);
		cout << "OnMessageResult_HttpGet: url:" << result->url
			<< " SN:" << result->serial_number
			<< " result:" << result->result
			<< endl;
	}

	void OnMessageResult_Login(net_comm::message::Message *message) {
		using namespace std;
		using namespace net_comm::message;

		const MessageLogin_Result *result = static_cast<const MessageLogin_Result *>(message);
		cout << "user-id: " << result->user_info.user_id << endl
			<< "sex: " << (result->user_info.sex ? "male" : "female") << endl
			<< "address.extadd: " << result->user_info.address.ext_address << endl
			<< "address.street: " << result->user_info.address.street << endl
			<< "address.locality: " << result->user_info.address.locality << endl
			<< "address.regin: " << result->user_info.address.regin << endl
			<< "address.post_code: " << result->user_info.address.post_code << endl
			<< "address.country: " << result->user_info.address.country << endl
			<< "nick_name: " << result->user_info.nick_name << endl
			<< "birthday: " << result->user_info.birthday << endl
			<< "head_image: " << result->user_info.head_image << endl
			<< endl;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace net_comm;
	using namespace net_comm::message;

	std::string name = "flaght@gmail.com";
	std::string password = "123456";
	std::string host ="app.miglab.com";
	std::string entry = "miglab";
	//MigInit(host,entry);
	//MigUsrLogin(name,password,host);

	g_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	Test test;
	net_comm::MigHttpModuleInterface *http_module = CreateHttpModuleInst();
 	http_module->Init(host, entry, &test);
 	net_comm::MessageDispatcher *disp = http_module->GetMessageDispatcher();

	disp->RegisterMessageHandler(net_comm::message::MID_HTTP_GET, test, &Test::OnMessageResult_HttpGet);
	disp->RegisterMessageHandler(net_comm::message::MID_LOGIN, test, &Test::OnMessageResult_Login);

// 	net_comm::message::MessageHttpGet_Launch get;
// 	get.serial_number = 0;
// 	get.url = "www.baidu.com";
// 	http_module->PushMessage(get);
// 	get.serial_number = 1;
// 	get.url = "www.google.com";
// 	http_module->PushMessage(get);
// 	get.serial_number = 2;
// 	get.url = "www.sina.com.cn";
// 	http_module->PushMessage(get);

	MessageLogin_Launch *login = new MessageLogin_Launch();
	login->name = name;
	login->password = password;
	login->host = host;
	http_module->PushMessage(login);

	while (1) {
		::WaitForSingleObject(g_event, INFINITE);
		disp->DoMessageDispatch();
	}

	http_module->Uninit();
	http_module->Release();
	::CloseHandle(g_event);

	return 0;
}

