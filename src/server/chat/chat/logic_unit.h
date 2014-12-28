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


static char *default_group_name = "临时讨论组";
static char *default_group_url = "http://face.miu.miglab.com/default.jpg";
static char *sound_url = "http://sound.miglab.com/";

#define DEFAULT_CONFIG_PATH     "./plugins/chat_manager/chat_manager_config.xml";

#define ALONE_CHAT              1
#define TEMP_GROUP_CHAT         2
#define DIMENSION_GROUP_CHAT    3


typedef std::map<int64/*group_id*/,chat_base::GroupInfo/*groupinfo*/> GroupInfosMap;

typedef std::map<int64/* user_id */,chat_base::UserInfo/* userinfo*/> UserInfosMap;

typedef std::map<int64 /*to_id*/,int64 /*session*/> SessionInfosMap;

typedef std::map<int64 /*uid*/,SessionInfosMap> LeaveInfosMap;

typedef std::map<int64,int64> MeetingSession;

typedef std::map<int64 /*session*/,MeetingSession> MeetingMap;

typedef std::map<int /*socket*/,chat_base::UserInfo/* userinfo*/> SocketMap;

typedef std::map<int64 /*msg id*/,chat_base::UserInfo /*userinfo*/> ConfirmMap;

#define MAX_SUMMARY_CONTENT_LEN  16

enum MSG_TYPE{
	TYPE_TEXT = 1,
	TYPE_PIC = 2,
	TYPE_SOUND = 3
};

enum RECORDTYPE{
	PARENT_TYPE = 1,
	SAYHELLO_TYPE = 2,
	MESSAGE_TYPE = 3
};

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


}
#endif
