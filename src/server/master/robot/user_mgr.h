#ifndef _MASTER_PLUGIN_ROBOT_USER_MGR__H__
#define _MASTER_PLUGIN_ROBOT_USER_MGR__H__
#include "logic_comm.h"
#include "robot_basic_infos.h"
#include "plugins.h"
#include "protocol/http_packet.h"
#include "json/json.h"
#include <list>
namespace robot_logic{

class UserMgr{
public:
	UserMgr();
	virtual ~UserMgr();
public:
	bool GetUserInfoMail(const int socket,const packet::HttpPacket& packet);

	bool GetRobotsInfo(const int socket,const packet::HttpPacket& packet);

private:
	void GetUserInfoMail(std::list<robot_base::MailUserInfo>& list, Json::Value& value);
	void GetRobotInfo(std::list<robot_base::RobotInfo>& list, Json::Value& value);
	bool CheckUserInfo(const std::string& username);

	std::string PrepNode(const std::string str, std::string::const_iterator start,
		    std::string::const_iterator end, bool *valid);

	std::string PrepDomain(const std::string str, std::string::const_iterator start,
	    std::string::const_iterator end, bool *valid);

	void PrepDomain(const std::string str, std::string::const_iterator start,
		    std::string::const_iterator end, std::string *buf, bool *valid);

	void PrepDomainLabel(const std::string str, std::string::const_iterator start,
		    std::string::const_iterator end, std::string *buf, bool *valid) ;

	char PrepNodeAscii(char ch, bool *valid);

	char PrepDomainLabelAscii(char ch, bool *valid) ;
};
}
#endif
