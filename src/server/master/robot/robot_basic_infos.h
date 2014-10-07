#ifndef _ROBOT_BASIC_INFOS_H__
#define _ROBOT_BASIC_INFOS_H__

#include "logic_comm.h"
#include <map>
#include <string>

#include "basic/basictypes.h"

#define DUMP_STRING(v) \
	LOG_DEBUG2("name %s---value %s",#v,v.c_str());

#define DUMP_INT(v) \
	LOG_DEBUG2("name %s---value %d",#v,v);

namespace robot_base{

class RobotInfo{
public:
	explicit RobotInfo();
	explicit RobotInfo(const int64 uid,const std::string& nickname,const std::string& sex,
				const std::string& head_url);

	RobotInfo(const RobotInfo& robot_info);
	RobotInfo& operator=(const RobotInfo& robotinfo);

	const int64 uid() const {return data_->uid_;}
	const std::string& nickname() const {return data_->nickname_;}
	const std::string& sex() const {return data_->sex_;}
	const std::string& head_url() const {return data_->head_url_;}

private:
	class Data{
	public:
		Data():uid_(0)
			,refcount_(1){}
		Data(const int64 uid,const std::string& nickname,const std::string& sex,
				const std::string& head_url)
			:refcount_(1)
			,uid_(uid)
			,nickname_(nickname)
			,sex_(sex)
			,head_url_(head_url){}
		void AddRef(){refcount_++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		const int64 uid_;
		std::string nickname_;
		std::string sex_;
		std::string head_url_;
		std::map<int64,int32>  say_hello_map_;
		std::map<int64,int32>  send_music_map_;
		std::map<int64,int32>  leave_mgr_map_;

	private:
		int            refcount_;
	};
	Data*                    data_;
};


class MailUserInfo{
public:
	explicit MailUserInfo();
	explicit MailUserInfo(const int64 uid,const std::string& username,const std::string& nickname);

	MailUserInfo(const MailUserInfo& mail_userinfo);
	MailUserInfo& operator = (const MailUserInfo& mail_userinfo);

	const int64 uid() const {return data_->uid_;}
	const std::string& username() const {return data_->username_;}
	const std::string& nickname() const {return data_->nickname_;}

private:
	class Data{
	public:
		Data():refcount_(1){}
		Data(const int64 uid,const std::string& username,
				const std::string& nickname)
			:refcount_(1)
			,username_(username)
			,nickname_(nickname){}
		void AddRef(){refcount_++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		int64          uid_;
		std::string    username_;
		std::string    nickname_;
	private:
		int            refcount_;
	};

	Data*                    data_;
};

class NewMusicInfo{
public:
	explicit NewMusicInfo();
	explicit NewMusicInfo(const int64 id,const std::string& name,
		         const std::string& singer);

	NewMusicInfo(const NewMusicInfo& platform_info);
	NewMusicInfo& operator = (const NewMusicInfo& platform_info);

	const std::string& name() const{
		return data_->name_;
	}

	const std::string& singer() const{
		return data_->singer_;
	}

	const int64& id() const {
		return data_->id_;
	}

	void set_name(const std::string& name){data_->name_ = name;}
	void set_singer(const std::string& singer) {data_->singer_ = singer;}
	void set_id(const int64 id) {data_->id_ = id;}
	void UnserializedJson(std::string& str);


private:
	class Data{
	public:
		Data():refcount_(1){}
		Data(const int64 id,const std::string& name,
		         const std::string& singer)
			:refcount_(1)
			,id_(id)
			,name_(name)
			,singer_(singer){}

		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		int64          id_;
		std::string    name_;
		std::string    singer_;
	private:
		int                  refcount_;	
	};

	Data*                    data_;
};

class SINAWBAccessToken{
public:
	explicit SINAWBAccessToken();

	explicit SINAWBAccessToken(const std::string& appkey,const std::string& appsecret,
			const std::string& callback,const int32 type);

	SINAWBAccessToken(const SINAWBAccessToken& access_token);
	SINAWBAccessToken& operator = (const SINAWBAccessToken& access_token);

	const std::string& appkey() const {return data_->appkey_;}
	const std::string& appsecret() const {return data_->appsecret_;}
	const std::string& callback() const {return data_->callback_;}
	const std::string& access_token() const {return data_->access_token_;}
	const int32 type() const {return data_->type_;}
	const int64 count() const {return data_->count_;}

	void add_count(){__sync_fetch_and_add(&data_->count_,1);}

	void set_access_token(const std::string& access_token){data_->access_token_ = access_token;}

	//重载比大于 小于符号
	bool operator < (SINAWBAccessToken& access_token);
	bool operator > (SINAWBAccessToken& access_token);

	static bool cmp(SINAWBAccessToken* t_access_token,SINAWBAccessToken* r_access_token);

	void Dump();


public:
	class Data{
	public:
		Data():refcount_(1)
			,type_(0)
			,count_(0){}
		Data(const std::string& appkey,const std::string& appsecret,
				const std::string& callback,const int32 type)
			:refcount_(1)
			,appkey_(appkey)
			,appsecret_(appsecret)
			,callback_(callback)
			,type_(type)
			,count_(0){}

		void AddRef(){__sync_fetch_and_add(&refcount_,1);}
		void Release(){
			  if ((__sync_fetch_and_sub(&refcount_,1)-1)==0)
				  delete this;
		}

	public:
		std::string  appkey_;
		std::string  appsecret_;
		std::string  callback_;
		std::string  access_token_;
		int32        type_;
		int64        count_;
	private:
		int                  refcount_;
	};
	Data*                    data_;
};

}
#endif
