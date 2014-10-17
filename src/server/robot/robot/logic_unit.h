#ifndef __ROBOT_LOGIC_UNIT_H__
#define __ROBOT_LOGIC_UNIT_H__

#include "robot_basic_info.h"
/*#include "dic_comm.h"*/
#include "log/mig_log.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include "plugins.h"
#include <map>
#include <list>

static char *PLUGIN_ID = "robot_manager";
static char *PLUGIN_NAME = "robot_manager";
static char *PLUGIN_VERSION = "1.0.0";
static char *PLUGIN_PROVIDER = "kerry";



#define DEFAULT_CONFIG_PATH     "./plugins/robot_manager/robot_manager_config.xml";

typedef std::map<int64,robot_base::RobotBasicInfo> RobotInfosMap;
typedef std::map<int64,RobotInfosMap> UserFollowMap;
typedef std::map<int,robot_base::SchedulerInfo> SchedulerMap;
typedef std::map<int64,robot_base::UserBasicInfo> UserBasicMap;
typedef std::map<int64,robot_base::UserInfo> UserInfoMap;
typedef std::map<int,robot_base::RobotBasicInfo>  SocketRobotInfosMap;
typedef std::map<int,robot_base::SchedulerInfo>  SocketSchedulerMap;

//
typedef std::map<int,robot_base::LuckGiftInfo* >      LuckGiftInfoPlatMap;
typedef std::map<int,LuckGiftInfoPlatMap>           LuckGiftInfoMap;

//便于不可之前场景冲突 故+100

enum {
	RAIN = 100,
	CLEAR_DAY = 101,
	CLEAR_NIGHT = 102,
	PARTLY_CLOUDY_DAY = 103,
	PARTLY_CLOUDY_NIGHT = 104,
	CLOUDY = 105,
	SLEET = 106,
	SNOW = 107,
	WIND = 108,
	FOG = 109,
	UNKONW
};

int caiyuncode(const char* status);
const char* codotodesc(const int32 code);
namespace robot_logic {
class ResolveJson{
public:
	static bool ReolveJsonCaiYunJson(const std::string& content,std::string& status,
			std::string& skycon,int32& temp,std::list<double>& dataseries_list);

	static bool ReolveJsonBaiduAddress(const std::string& content,std::string& city,
			std::string& district,std::string& province,std::string& street);
};

class LogicUnit{
public:
	static void FormateMusicInfo(std::list<std::string>& songinfolist,
				 std::map<std::string,base::MusicInfo>& music_infos);

	static void CalculateOneHourWeather(std::list<double>& dataseries_list,
			double& weather_result,int& furture_time);

	static int ResultWeather(const double result);
};
/*
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
*/

}

#endif
