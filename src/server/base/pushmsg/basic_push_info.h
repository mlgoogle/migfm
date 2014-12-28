#ifndef _MIG_FM_BASIC_PUSH_INFO_H__
#define _MIG_FM_BASIC_PUSH_INFO_H__
#include "basic/base64.h"
#include "json/json.h"
#include <string>
#include <map>
#include <sstream>
#include <time.h>

enum notification_style{
	CLEAR_STYLE = 0x01,
	VIBRATE_STYLE = 0x02,
	RING_STYLE = 0x04
};

enum notification_type{
	OPEN_URL = 1,
	OPEN_APP = 2
};

enum machine_type{
	ALL_TYPE = 0,
	IOS_TYPE = 1,
	ANDROID_TYPE = 2
};

namespace base_push{

class BaiduBindPushInfo{
public:
	explicit BaiduBindPushInfo();
	explicit BaiduBindPushInfo(const int64 platform,const int64 uid,const int64 channel,const int64 bd_userid,
			const std::string& pkg_name,const std::string& tag,const int64 appid,
			const int64 requestid,const int32 machine);
	explicit BaiduBindPushInfo(const BaiduBindPushInfo& bind);
	BaiduBindPushInfo& operator = (const BaiduBindPushInfo& bind);

	const int64 platform() const{return data_->platform_;}
	const int64 uid() const{return data_->uid_;}
	const int64 channel() const{return data_->channel_;}
	const int64 bd_userid() const {return data_->bd_userid_;}
	const std::string& pkg_name() const{return data_->pkg_name_;}
	const std::string& tag() const {return data_->tag_;}
	const int64 appid() const {return data_->appid_;}
	const int64 requestid() const{return data_->requestid_;}
	const int32 machine() const {return data_->machine_;}

public:
	class Data{
	public:
		Data(const int64 platform,const int64 uid,const int64 channel,const int64 bd_userid,
				const std::string& pkg_name,const std::string& tag,const int64 appid,
				const int64 requestid,const int32 machine)
	:refcount_(1),platform_(platform),uid_(uid),channel_(channel),bd_userid_(bd_userid)
	,pkg_name_(pkg_name),tag_(tag),appid_(appid),requestid_(requestid),machine_(machine){}

	Data():refcount_(1),platform_(0),uid_(0),channel_(0),bd_userid_(0)
		,appid_(0),requestid_(0),machine_(0){}
	public:
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		int64                        platform_;
		int64                        uid_;
		int64                        channel_;
		int64                        bd_userid_;
		std::string                  pkg_name_;
		std::string                  tag_;
		int64                        appid_;
		int64                        requestid_;
		int32                        machine_;
	private:
		int refcount_;
	};
	Data*                    data_;
};

class BaiduPushCustomContent{
public:
	explicit BaiduPushCustomContent();
	explicit BaiduPushCustomContent(const BaiduPushCustomContent& custom);
	BaiduPushCustomContent& operator = (const BaiduPushCustomContent& custom);

    ~BaiduPushCustomContent(){
        if(data_!=NULL){
            data_->Release();
        }
    }

    void set_name(const std::string& name){data_->name_ = name;}

	void set_value(const std::string& key,const std::string& value);
	void set_value(const std::string& key,const int64 value);
	void set_value(const std::string& key,const int32 value);

	inline int get_count(){return data_->param_map_.size();}
	void GetJsonObject(Json::Value& result);

private:
	class Data{
	public:
		Data():refcount_(1){}
	public:
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		std::string                       name_;
		std::map<std::string,std::string> param_map_;
	private:
		int refcount_;
	};
	Data*                    data_;
};

class BaiduPushMessage{
public:
	explicit BaiduPushMessage();
	explicit BaiduPushMessage(const int64 platform,const int64 uid);

	explicit BaiduPushMessage(const int64 platform,const int64 uid,const int32 level,const int64 msg_id = 0);


    ~BaiduPushMessage(){
        if(data_!=NULL){
            data_->Release();
        }
    }

	BaiduPushMessage(const BaiduPushMessage& push_message);
	BaiduPushMessage& operator = (const BaiduPushMessage& push_message);

	bool SerializedJson(std::string& content);

	void GetJsonObject(Json::Value& result);

	void set_title(const std::string& title) {
		//if(data_->machine_==1)
			data_->title_ = title;
		//else if(data_->machine_==2)
			set_ios_cunstom_content("alert",title);
	}

	const int machine() const {return data_->machine_;}


	void set_machine(const int machine) {data_->machine_ = machine;}
	void set_channel(const int64 channel) {data_->channel_ = channel;}
	void set_baidu_userid(const std::string& baidu_userid){data_->baidu_user_id_ = baidu_userid;}
	void set_description(const std::string& description) {data_->description_ = description;}
	void set_notification_builder_id(const int notification_builder_id) {data_->notification_builder_id_ = notification_builder_id;}
	void set_notification_basic_style(const int notification_basic_style){data_->notification_basic_style_ = notification_basic_style;}
	void set_url(const std::string url){data_->url_ = url;data_->open_type_ = 1;}
	void set_user_confirm(const int user_confirm) {data_->user_confirm_ = user_confirm;}
	void set_pkg_name(const std::string& pkg_name) {data_->pkg_name_ = pkg_name; }
	void set_tag(const std::string& tag){data_->tag_ = tag;}



	void set_value(const std::string& key,const std::string& value);
	void set_value(const std::string& key,const int64 value);
	void set_value(const std::string& key,const int32 value);


	const int64 channel() const {return data_->channel_;}
	const std::string& baidu_user_id() const {return data_->baidu_user_id_;}
	const int64& msg_id() const {return data_->msg_id_;}

	inline void set_android_cunstom_content(const std::string& key,const std::string& value){
		data_->android_cunstom_content_.set_value(key,value);
	}

	inline void set_android_cunstom_content(const std::string& key,const int64 value){
		data_->android_cunstom_content_.set_value(key,value);
	}

	inline void set_android_cunstom_content(const std::string& key,const int32 value){
		data_->android_cunstom_content_.set_value(key,value);
	}

	inline void set_ios_cunstom_content(const std::string& key,const std::string& value){
		data_->ios_cunstom_content_.set_value(key,value);
	}
	inline void set_ios_cunstom_content(const std::string& key,const int64 value){
		data_->ios_cunstom_content_.set_value(key,value);
	}
	inline void set_ios_cunstom_content(const std::string& key,const int32 value){
		data_->ios_cunstom_content_.set_value(key,value);
	}

private:
	class Data{
	public:
		Data():refcount_(1)
			,platform_(0)
			,uid_(0)
			,machine_(ANDROID_TYPE)
			,notification_builder_id_(0)
			,notification_basic_style_(VIBRATE_STYLE|RING_STYLE)
			,open_type_(OPEN_APP)
			,net_support_(1)
			,user_confirm_(0)
			,channel_(0)
			,msg_id_(0){

			android_cunstom_content_.set_name("custom_content");
			ios_cunstom_content_.set_name("aps");
		}

		Data(const int64 platform,const int64 uid):refcount_(1)
			,platform_(platform)
			,uid_(uid)
			,machine_(ANDROID_TYPE)
			,notification_builder_id_(0)
			,notification_basic_style_(VIBRATE_STYLE|RING_STYLE)
			,open_type_(OPEN_APP)
			,net_support_(1)
			,user_confirm_(0)
			,channel_(0)
			,msg_id_(0){

			android_cunstom_content_.set_name("custom_content");
			ios_cunstom_content_.set_name("aps");
		}

		Data(const int64 platform,const int64 uid,const int32 level,const int64 msg_id):refcount_(0)
			,platform_(platform)
			,uid_(uid)
			,machine_(ALL_TYPE)
			,notification_builder_id_(0)
			,open_type_(OPEN_APP)
			,net_support_(1)
			,user_confirm_(0)
			,channel_(0)
			,msg_id_(msg_id){

			android_cunstom_content_.set_name("custom_content");
			ios_cunstom_content_.set_name("aps");
			notification_basic_style_ = level;
			//设置IOS
			if(level !=VIBRATE_STYLE)//除此之外都需要振动
				ios_cunstom_content_.set_value("sound","");

		}
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		const int64              platform_;
		const int64              uid_; // 群发时为0
		int64                    channel_;//群发时为0
		const int64              msg_id_;
		std::string              baidu_user_id_;
		std::string              tag_;
		int                      machine_;//机器类型 0,两者兼容 1,IOS 2, android 默认1
		std::string              title_;
		std::string              description_;
		int                      notification_builder_id_;//android 消息格式 默认为0
		int                      notification_basic_style_;//
		int                      open_type_;//打开行为即消息类型 1,打开URL 2,打开应用
		int                      net_support_;
		std::string              url_;
		int                      user_confirm_;// open_type 要为1 1，打开URL 需要用户允许，0，打开URL 不需要用户允许
		std::string              pkg_content_;// open_type 要为2
		std::string              pkg_name_;//app 包名
		BaiduPushCustomContent   android_cunstom_content_;//安卓自定义消息
		BaiduPushCustomContent   ios_cunstom_content_;//ios 特定消息
		std::map<std::string,std::string>      param_map_;//IOS 自定义字段
	private:
		int refcount_;
	};
	Data*                    data_;
};


class BaiDuPushMsgInfo{
public:
	explicit BaiDuPushMsgInfo();
	BaiDuPushMsgInfo(const BaiDuPushMsgInfo& push_msg_info);
	BaiDuPushMsgInfo& operator = (const BaiDuPushMsgInfo& push_msg_info);

    ~BaiDuPushMsgInfo(){
        if(data_!=NULL){
            data_->Release();
        }
    }

	void set_value(const std::string& key,const std::string& value);
	void set_value(const std::string& key,const int64 value);

	void set_method(const std::string& method){data_->method_ = method;}
	void set_url(const std::string& url){data_->url_ = url;}
	void set_secret(const std::string& secret){data_->secret_ = secret;}

	void GetSign(std::string& hexdigest);

	void GetContent(std::string& content);

private:
	void Calculate();
	void Package();

private:
	class Data{
	public:
		Data():refcount_(0){}
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		std::string method_;
		std::string url_;
		std::string secret_;
		std::map<std::string,std::string> param_map_;
	private:
		int refcount_;
	};
	Data*                    data_;
	std::string              hexdigest_;
	std::string              content_;
	bool                     sign_change_;
	bool                     data_change_;
};

}
#endif
