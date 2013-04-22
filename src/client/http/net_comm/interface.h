#ifndef _HTTP_NET_COMM_INTER_FACE_H__
#define _HTTP_NET_COMM_INTER_FACE_H__

#define NET_Export _declspec(dllexport) __stdcall

#include <string>
#include "sigslot/sigslot.h"

extern "C" int NET_Export MigGetHttpTest();
extern "C" int NET_Export MigInit(std::string& host,std::string& entry);
extern "C" int NET_Export MigUsrLogin(std::string& username,std::string& passwd,std::string& host);

////////////////////////////////////////////////////////////////////////////////
#define NET_COMMON_API	__declspec(dllexport)
#define STD_CALL		__stdcall

////////////////////////////////////////////////////////////////////////////////

namespace net_comm {
class MigHttpModuleInterface;
class MigHttpModuleCallbackInterface;
class MessageDispatcher;

namespace message {
	class Message;
} // namespace message

// Http请求处理器
class MigHttpModuleInterface {
public:
	virtual ~MigHttpModuleInterface() = 0 {}

	//************************************
	// Method:    Release
	// FullName:  MigHttpModuleInterface::Release
	// Access:    virtual public 
	// Returns:   void
	// Qualifier: 释放对象
	//************************************
	virtual void Release() = 0;

	//************************************
	// Method:    Init
	// FullName:  MigHttpModuleInterface::Init
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier: 初始化通信对象
	// Parameter: const std::string & host:		主机名		
	// Parameter: const std::string & entry:	
	// Parameter: MigHttpModuleCallbackInterface *cb: 请求完成回调接口
	// Parameter: const void *reserved:			扩展参数
	//************************************
	virtual bool Init(const std::string& host, const std::string& entry,
					  MigHttpModuleCallbackInterface *cb, const void *reserved=NULL) = 0;

	//************************************
	// Method:    Uninit
	// FullName:  MigHttpModuleInterface::Uninit
	// Access:    virtual public 
	// Returns:   void
	// Qualifier: 清理对象
	//************************************
	virtual void Uninit() = 0;

	//************************************
	// Method:    PushMessage
	// FullName:  net_comm::MigHttpModuleInterface::PushMessage
	// Access:    virtual public 
	// Returns:   void
	// Qualifier: 推送消息
	// Parameter: Message * message: 消息对象
	//************************************
	virtual void PushMessage( message::Message *message) = 0;


	//************************************
	// Method:    GetMessageDispatcher
	// FullName:  net_comm::MigHttpModuleInterface::GetMessageDispatcher
	// Access:    virtual public 
	// Returns:   MessageDispatcher *
	// Qualifier: 获取消息调度器指针
	//************************************
	virtual MessageDispatcher *GetMessageDispatcher() = 0;
};

// Http请求处理回调
class MigHttpModuleCallbackInterface {
public:
	virtual ~MigHttpModuleCallbackInterface() = 0 {}

	//************************************
	// Method:    OnHttpRequestCompleted
	// FullName:  MigHttpModuleCallbackInterface::OnHttpRequestCompleted
	// Access:    virtual public 
	// Returns:   void
	// Qualifier: http请求完成处理函数
	//************************************
	virtual void OnHttpRequestCompleted() = 0;
};

} // namespace net_comm

//************************************
// Method:    CreateInst
// FullName:  CreateInst
// Access:    public 
// Returns:   extern "C" MigHttpModuleInterface * NET_Export
// Qualifier: 创建Http处理对象
//************************************
extern "C" NET_COMMON_API net_comm::MigHttpModuleInterface * STD_CALL CreateHttpModuleInst();


#endif