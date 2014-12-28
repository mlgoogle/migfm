#ifndef _MIG_FM_LBS_BAIDU_CONNECTOR_H__
#define _MIG_FM_LBS_BAIDU_CONNECTOR_H__
#include "push_connector.h"
#include "basic_push_info.h"
#include "basic/basic_info.h"
#include "protocol/http_packet.h"
#include <list>
#include <string>

#if defined _DEBUG || defined DEBUG
#define DEPLOY_STATUS         1
#else
#define DEPLOY_STATUS         2
#endif
namespace base_push{


enum DEVICE_TYPE{
	BROWN = 1,
	PC = 2,
	ANDRIOD = 3,
	IOS = 4,
	WP = 5
};


class PushBaiduConnectorImpl:public PushConnector{

public:
	PushBaiduConnectorImpl();
	virtual ~PushBaiduConnectorImpl();

public:
	virtual void Init(std::list<base::ConnAddr>& addrlist);
	virtual void Release();
public:

	void PushUserMessage(const int64 platform,const int64 uid,const std::string& title,
			const std::string& description,
			const int32 level = SOUND_TYPE|VIBRATE_TYPE|CLEAR_TYPE,
			const int32 badge = 1);//推送指定用户
	void PushAllUserMessage(const std::string& title,const std::string& description,
			const int32 level = SOUND_TYPE|VIBRATE_TYPE|CLEAR_TYPE,
			const int32 badge = 1);//推送所有用户

	bool BindPushUserinfo(packet::HttpPacket& packet,int32& err,std::string& err_str); //绑定推送用户信息

	bool BindPushUserinfo(base_push::BaiduBindPushInfo& push_info);

	void TimeDBPushMessage();
private:

	bool PushUserMessage(const std::string& url,const int32 push_type,
			base_push::BaiduPushMessage& push_message);

	bool PushUserMessage(const std::string& url,
			const std::string& userid,const int64 channel,
			const std::string& title,const std::string& description);

	void MakePushMessage(const std::string& title,const std::string& description,std::string& message);


	bool PushBaiduMessage(const std::string& url,const std::string& content);

	bool PushBaiduMsg(const std::string& url,const std::string& content,std::string& reponse);

	void TestSign();

private:
	std::string            host_;
	std::string            key_;
	std::string            secret_;
	std::string            version_;
	std::string            channel_;

};

}
#endif
