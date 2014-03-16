#ifndef __CHAT_LOGIC_UNIT_H__
#define __CHAT_LOGIC_UNIT_H__

#include "chat_basic_infos.h"
#include "dic_comm.h"
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "plugins.h"
#include <map>

static char *PLUGIN_ID = "chat_manager";
static char *PLUGIN_NAME = "chat_manager";
static char *PLUGIN_VERSION = "1.0.0";
static char *PLUGIN_PROVIDER = "kerry";

#define DEFAULT_CONFIG_PATH     "./plugins/chat_manager/chat_manager_config.xml"

typedef std::map<int64/* user_id */,chat_base::UserInfo/* userinfo*/> UserInfosMap;

typedef std::map<int64 /*to_id*/,int64 /*session*/> SessionInfosMap;

typedef std::map<int64 /*uid*/,SessionInfosMap> LeaveInfosMap;

typedef std::map<int64,int64> MeetingSession;

typedef std::map<int64 /*session*/,MeetingSession> MeetingMap;

typedef std::map<int /*socket*/,chat_base::UserInfo/* userinfo*/> SocketMap;


namespace chat_logic {

class LogicUnit{
public:

	static bool GetUserInfo(const int64 platform_id,int64 user_id,
				 chat_base::UserInfo& userinfo);

	static bool CheckToken(const int64 platform_id,int64 user_id,
			const char* token);


};

class HttpComm {
public:
	static bool PushMessage(const std::string &device_token, const std::string &msg,
			int badge=1, const std::string &sound="");
};


}
#endif
