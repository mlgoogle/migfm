#ifndef _MESSAGE_H_
#define _MESSAGE_H_

////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include "base/basictypes.h"

namespace net_comm {
namespace message {

////////////////////////////////////////////////////////////////////////////////
enum MessageID {
	MID_HTTP_GET	= 0,
	MID_HTTP_POST	= 1,

	MID_LOGIN		= 10,
};

////////////////////////////////////////////////////////////////////////////////
class Message {
public:
	int id() const { return id_; }

protected:
	explicit Message(int id) : id_(id) {}
public:
	virtual ~Message() {}

protected:
	const int				id_;
};

////////////////////////////////////////////////////////////////////////////////
class MessageHttpGet_Launch : public Message {
public:
	MessageHttpGet_Launch() : Message(MID_HTTP_GET), serial_number(0) {}

	int	serial_number;
	std::string url;
};

class MessageHttpGet_Result : public Message {
public:
	MessageHttpGet_Result() : Message(MID_HTTP_GET), serial_number(0) {}

	int	serial_number;
	std::string url;
	std::string result;
};

////////////////////////////////////////////////////////////////////////////////
class MessageHttpPost_Launch : public Message {
public:
	MessageHttpPost_Launch() : Message(MID_HTTP_POST), serial_number(0) {}

	int	serial_number;
	std::string url;
	std::string post;
};

class MessageHttpPost_Result : public Message {
public:
	MessageHttpPost_Result() : Message(MID_HTTP_POST), serial_number(0) {}

	int	serial_number;
	std::string url;
	std::string result;
};

////////////////////////////////////////////////////////////////////////////////
class MessageLogin_Launch : public Message {
public:
	MessageLogin_Launch() : Message(MID_LOGIN) {}

	std::string host;
	std::string name;
	std::string password;
};

struct UserInfo {
	int64	user_id;
	int8	sex;

	std::string nick_name;
	std::string birthday;
	std::string	head_image;

	struct {
		std::string ext_address;
		std::string street;
		std::string locality;
		std::string regin;
		std::string post_code;
		std::string country;
	} address;

	UserInfo() : user_id(0), sex(0) {}
};

class MessageLogin_Result : public Message {
public:
	MessageLogin_Result() : Message(MID_LOGIN) {}

	UserInfo user_info;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace message
} // namespace net_comm

#endif // _MESSAGE_H_
