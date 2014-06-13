#ifndef __ROBOT_LOGIC_UNIT_H__
#define __ROBOT_LOGIC_UNIT_H__

#include "robot_basic_info.h"
/*#include "dic_comm.h"*/
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "plugins.h"
#include <map>

static char *PLUGIN_ID = "robot_manager";
static char *PLUGIN_NAME = "robot_manager";
static char *PLUGIN_VERSION = "1.0.0";
static char *PLUGIN_PROVIDER = "kerry";



#define DEFAULT_CONFIG_PATH     "./plugins/robot_manager/robot_manager_config.xml";

typedef std::map<int64,robot_base::RobotBasicInfo> RobotInfosMap;

/*
namespace chat_logic {

class LogicUnit{
public:

	static bool GetUserInfo(const int64 platform_id,int64 user_id,
				 chat_base::UserInfo& userinfo);

	static bool CheckToken(const int64 platform_id,int64 user_id,
			const char* token);

	static bool SetChatToken(chat_base::UserInfo& userinfo);

	static bool CheckChatToken(const chat_base::UserInfo& userinfo,const char* token);

	static void GetCurrntTimeFormat(std::string& current_time);

	static void GetCurrentTimeFormat(const time_t current,std::string& current_time);

	static bool MakeLeaveContent(const std::string& send_uid,const std::string& to_uid,
			int64 msg_id,const std::string& msg,std::string& detail,std::string &summary,
			std::string& current);

	static bool MakeLeaveContent(const chat_base::UserInfo& send_userinfo,
			               const chat_base::UserInfo& recv_userinfo,const std::string& msg,
			               const int msg_type,std::string &summary,double& distance);




};

class HttpComm {
public:
	static bool PushMessage(const std::string &device_token, const std::string &msg,
			int badge=1, const std::string &sound="");
};


class SummaryContent{
public:
	static bool OnSummaryContent(const chat_base::UserInfo& send_userinfo,
			                     const chat_base::UserInfo& recv_userinfo,
					             const std::string& msg,const int msg_type,
					             std::string & summary);
private:
	static bool SummaryTestContent(const chat_base::UserInfo& send_userinfo,
            const chat_base::UserInfo& recv_userinfo,const std::string& msg,
            std::string & summary);
};


}*/

#endif
