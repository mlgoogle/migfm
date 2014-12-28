#ifndef _MIG_FM_WEATHER_ENGINE_H__
#define _MIG_FM_WEATHER_ENGINE_H__

#include "basic/basic_info.h"
#include <list>

namespace base_weather{
enum StorageImplType{
	IMPL_CAIYUN = 0,
	IMPL_MOJI = 1
};

class WeatherConnector{
public:
	static WeatherConnector* Create(int32 type);
	virtual ~WeatherConnector(){}
public:
	virtual void Init() = 0;//初始化
	virtual void Release() = 0;//释放
	virtual bool GetWeatherInfo(const std::string& latitude,const std::string& longitude,std::string&temp,
			std::string& current_weather) = 0;
	virtual bool GetWeatherInfo(const double latitude,const double longitude,std::string& temp,
			std::string& current_weather) = 0;
};

class WeatherConnectorEngine{
public:
	WeatherConnectorEngine() {}
	virtual ~WeatherConnectorEngine() {}

	static void Create(int32 type){
		weather_connector_engine_ = WeatherConnector::Create(type);
	}

	static WeatherConnector* GetWeatherConnectorEngine (){
		return weather_connector_engine_;
	}

	static void FreeWeatherConnectorEngine(){
		delete weather_connector_engine_;
	}

private:
	static WeatherConnector   *weather_connector_engine_;
};

}

#endif
