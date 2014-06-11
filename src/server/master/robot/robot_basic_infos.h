#ifndef _CHAT_BASIC_INFOS_H__
#define _CHAT_BASIC_INFOS_H__

#include <map>
#include <string>

#include "basic/basictypes.h"
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

}
#endif
