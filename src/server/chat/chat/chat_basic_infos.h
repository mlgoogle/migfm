#ifndef _CHAT_BASIC_INFOS_H__
#define _CHAT_BASIC_INFOS_H__

#include <map>
#include <string>

#include "basic/basictypes.h"
namespace chat_base{


class GroupInfo{
public:
	explicit GroupInfo();
	explicit GroupInfo(const int64 platform_id,const int64 group_id,const int16 group_type,
						const int64 nicknumber,const int64 session,const std::string& name,
						const std::string& head_url);

	GroupInfo(const GroupInfo& group_info);
	GroupInfo& operator = (const GroupInfo& group_info);

	void set_nicknumber(const int64 nicknumber) {data_->nicknumber_ = nicknumber;}
	void set_name(const std::string& name) {data_->name_ = name;}
	void set_head_url(const std::string& head_url) {data_->head_url_ = head_url;}
	void set_session(const int64 session) {data_->session_ = session;}

	const int64 groupid() const {return data_->group_id_;}
	const int64 nicknumber() const {return data_->nicknumber_;}
	const std::string& name() const {return data_->name_;}
	const std::string& head_url() const {return data_->head_url_;}
	const int64 session() const {return data_->session_;}

private:
	class Data{
	public:
		Data():refcount_(1)
			,platform_id_(0)
			,group_id_(0)
			,group_type_(0)
			,nicknumber_(0){}

		Data(const int64 platform_id,const int64 group_id,const int16 group_type,
				const int64 nicknumber,const int64 session,const std::string& name,
				const std::string& head_url):refcount_(1)
			,platform_id_(platform_id),group_id_(group_id)
			,group_type_(group_type),session_(session),nicknumber_(nicknumber)
			,name_(name),head_url_(head_url){}

		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}

	public:
		const int64 platform_id_;
		const int64 group_id_;
		const int16 group_type_;
		int64  nicknumber_;
		int64  session_;
		std::string name_;
		std::string head_url_;
	private:
		int refcount_;
	};
	Data*                    data_;

};

class UserInfo{
public:
	explicit UserInfo();

	explicit UserInfo(const int64 platform_id,const int64 user_id,
					const int64 nicknumber,const std::string& nickname,
					const std::string& head_url);

	explicit UserInfo(const int64 platform_id,const int64 user_id,
		              const int64 nicknumber,const int socket,const int64 session,
					  const std::string& nickname,const std::string& head_url,
					  const std::string& platform_token,
					  const std::string& chat_token);

	UserInfo(const UserInfo& user_info);
	UserInfo& operator = (const UserInfo& user_info);

	bool UnserializedJson(const char* str);

	void set_socket(int socket) {data_->socket_ = socket;}
	void set_session(int64 session) {data_->session_ = session;}
	void set_nickname(const std::string& nickname){data_->nickname_ = nickname;}
	void set_head_url(const std::string& head_url) {data_->head_url_ = head_url;}
	void set_platform_token(const std::string& platform_token){data_->platform_token_ = platform_token;}
	void set_chat_token(const std::string& chat_token) {data_->chat_token_ = chat_token;}


	const int64 platform_id() const {return data_->platform_id_;}
	const int64 user_id() const {return data_->user_id_;}
	const int64 nicknumber() const {return data_->nicknumber_;}
	const int64 session() const {return data_->session_;}
	const int socket() const {return data_->socket_;}
	const std::string& nickname() const {return data_->nickname_;}
	const std::string& head_url() const {return data_->head_url_;}
	const std::string& platform_token() const {return data_->platform_token_;}
	const std::string& chat_token() const {return data_->chat_token_;}



private:
	class Data{
	public:
		Data():refcount_(1)
			,platform_id_(0)
			,user_id_(0)
			,socket_(0)
			,session_(0)
			,nicknumber_(0){}

		Data(const int64 platform_id,const int64 user_id,
			  const int64 nicknumber,const std::string& nickname,
			  const std::string& head_url):refcount_(1)
				,platform_id_(platform_id),user_id_(user_id)
				,nicknumber_(nicknumber),socket_(0),session_(0),
				nickname_(nickname),head_url_(head_url){}

		Data(const int64 platform_id,const int64 user_id,
			const int64 nicknumber,const int socket,const int64 session,
			const std::string& nickname,const std::string& head_url,
			const std::string& platform_token,
			const std::string& chat_token):refcount_(1)
			,platform_id_(platform_id),user_id_(user_id)
			,nicknumber_(nicknumber),socket_(socket),session_(session)
			,nickname_(nickname),head_url_(head_url)
			,platform_token_(platform_token),chat_token_(chat_token){}

		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}

	public:
		const int64    platform_id_;
		const int64    user_id_;
		const int64    nicknumber_;
		int            socket_;
		int64          session_;
		std::string    nickname_;
		std::string    head_url_;
		std::string    platform_token_;
		std::string    chat_token_;
	private:
		int                  refcount_;	
	};

	Data*                    data_;
};


class PlatformInfo{
public:
	explicit PlatformInfo();
	explicit PlatformInfo(const int64 platform_id,
		         const std::string& platform_name);

	PlatformInfo(const PlatformInfo& platform_info);
	PlatformInfo& operator = (const PlatformInfo& platform_info);

private:
	class Data{
	public:
		Data():refcount_(1)
			,platform_id_(0){}
		Data(const int64 platform_id,const std::string& platform_name)
			:refcount_(1)
			,platform_id_(platform_id)
			,platform_name_(platform_name){}

		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	public:
		int64                platform_id_;
		const std::string    platform_name_;
	private:
		int                  refcount_;	
	};

	Data*                    data_;
};


class MessageInfos{
public:
	explicit MessageInfos();
	explicit MessageInfos(const int64 platform,const int64 msgid,const int64 fid,const int64 oppid,
			const int32 type,const std::string& nickname,const std::string& message,
			const std::string& lasstime,const std::string& head);
	MessageInfos(const MessageInfos& msginfo);
	MessageInfos& operator = (const MessageInfos& msginfo);
	const int64 platform() const {return data_->platform_;}
	const int64 msgid() const {return data_->msgid_;}
	const int64 fid() const {return data_->fid_;}
	const int64 oppid() const {return data_->oppid_;}
	const int64 type() const {return data_->type_;}
	const std::string& nickname() const {return data_->nickname_;}
	const std::string& message() const {return data_->message_;}
	const std::string& lasttime() const {return data_->lasttime_;}
	const std::string& head() const {return data_->head_;}

	void set_platform(const int64 platform) {data_->platform_ = platform;}
	void set_msgid(const int64 msgid) {data_->msgid_ = msgid;}
	void set_fid(const int64 fid) {data_->fid_ = fid;}
	void set_tid(const int64 oppid) {data_->oppid_ = oppid;}
	void set_group_id(const int32 type) {data_->type_ = type;}
	void set_nickname(const std::string& nickname){data_->nickname_ = nickname;}
	void set_message(const std::string& message){data_->message_ = message;}
	void set_lasttime(const std::string& lasttime){data_->lasttime_ = lasttime;}
	void set_head(const std::string& head){data_->head_ = head;}

public:
	class Data{
	public:
		Data()
		  :refcount_(1)
		  ,msgid_(0)
		  ,fid_(0)
		  ,oppid_(0)
		  ,type_(0)
		  ,platform_(0){}

		Data(const int64 platform,const int64 msgid,const int64 fid,const int64 oppid,
				const int32 type,const std::string& nickname,const std::string& message,
				const std::string& lasstime,const std::string& head)
		 :refcount_(1)
		  ,platform_(platform)
		  ,msgid_(msgid)
		  ,fid_(fid)
		  ,oppid_(oppid)
		  ,type_(type)
		  ,nickname_(nickname)
		  ,message_(message)
		  ,lasttime_(lasstime)
		  ,head_(head){}

	public:
		int64 platform_;
		int64 msgid_;
		int64 fid_;
		int64 oppid_;
		int32 type_;
		std::string nickname_;
		std::string message_;
		std::string lasttime_;
		std::string head_;
		void AddRef(){refcount_ ++;}
		void Release(){if (!--refcount_)delete this;}
	private:
		int refcount_;
	};
	Data*       data_;
};

}
#endif
