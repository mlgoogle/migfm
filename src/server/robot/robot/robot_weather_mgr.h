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
	bool PackageWeatherInfo(const int64 uid,const std::string& latitude,const std::string& longitude);
	bool RequestCaiYunWeather(const std::string& latitude,const std::string& longitude,int32& temp,
			int32& curret_weather,int32& future_weather);
	bool RequestBaiduAddress(const std::string& latitude,const std::string& longitude,
			std::string& city,std::string& district,std::string& province,std::string& street);
	bool CalculateOneHourWeather(std::list<double>& dataseries_list);
	bool GetWeatherMusic(const int type_id,int64& songid,std::string& singer,std::string& song);
};
}

#endif
