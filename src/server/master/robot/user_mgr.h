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

private:
	void GetUserInfoMail(std::list<robot_base::MailUserInfo>& list, Json::Value& value);
	bool CheckUserInfo(const std::string& username);
};
}
#endif
