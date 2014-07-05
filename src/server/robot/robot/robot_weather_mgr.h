#ifndef _ROBOT_ROBOT_WEATHER_MGR_H__
#define _ROBOT_ROBOT_WEATHER_MGR_H__
#include "robot_basic_info.h"
#include "logic_unit.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include "json/json.h"
#include "common.h"

namespace robot_logic{
class RobotWeatherMgr{
public:
	RobotWeatherMgr();
	virtual ~RobotWeatherMgr();

public:
	bool OnPullWeatherInfo(void);
private:
	bool PackageWeatherInfo(const int64 uid,const std::string& latitude,const std::string& longitude,
			int64& songid,std::string& message);
	bool RequestCaiYunWeather(const std::string& latitude,const std::string& longitude,int32& temp,
			int32& curret_weather,int32& future_weather,int32& futrue_time);
	bool RequestBaiduAddress(const std::string& latitude,const std::string& longitude,
			std::string& city,std::string& district,std::string& province,std::string& street);
	bool GetWeatherMusic(const int type_id,int64& songid,std::string& singer,std::string& song);


private:
	//原子计数来获取用户数
	int        get_user_from_tick_;
	int        get_user_count_tick_;
};
}

#endif
