#ifndef _MIG_FM_PUSH_CONNECTOR_H__
#define _MIG_FM_PUSH_CONNECTOR_H__

#include "basic/basic_info.h"
#include "basic/basictypes.h"
#include "pushmsg/basic_push_info.h"
#include "protocol/http_packet.h"
#include <list>

enum MSG_LEVEL{
	NO_TYPE = 0x01,
	SOUND_TYPE = 0x02,
	VIBRATE_TYPE = 0x03,
	CLEAR_TYPE = 0x04
};

namespace base_push{

enum PUSHImplType{
	IMPL_BAIDU = 0
};

class PushConnector{
public:
	static PushConnector* Create(int32 type);
	virtual ~PushConnector(){}
public:
	virtual void Init(std::list<base::ConnAddr>& addrlist) = 0;//初始化 //读取key secret

	virtual void PushUserMessage(const int64 platform,const int64 uid,const std::string& title,
			const std::string& description,
			const int32 level = SOUND_TYPE|VIBRATE_TYPE|CLEAR_TYPE,
			const int32 badge = 1) = 0;//推送指定用户

	virtual void PushAllUserMessage(const std::string& title,const std::string& description,
						const int32 level = SOUND_TYPE|VIBRATE_TYPE|CLEAR_TYPE,
						const int32 badge = 1) = 0;//推送所有用户
	virtual bool BindPushUserinfo(packet::HttpPacket& packet,int32& err,std::string& err_str)= 0; //绑定推送用户信息

	virtual bool BindPushUserinfo(base_push::BaiduBindPushInfo& push_info)= 0; //绑定推送用户信息

	virtual void TimeDBPushMessage() = 0; //定时读取数据库推送消息
	virtual void Release() = 0;//释放
};

class PushConnectorEngine{
public:
	PushConnectorEngine() {}
	virtual ~PushConnectorEngine() {}

	static void Create(int32 type){
		push_connector_engine_ = PushConnector::Create(base_push::IMPL_BAIDU);
	}

	static PushConnector* GetPushConnectorEngine (){
		return push_connector_engine_;
	}

	static void FreePushConnectorEngine  (){
		delete push_connector_engine_;
	}

private:
	static PushConnector   *push_connector_engine_;
};

}

#endif
