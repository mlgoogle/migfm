#ifndef _MIG_FM_CAIYUN_WEATHER_CONNECTOR_H__
#define _MIG_FM_CAIYUN_WEATHER_CONNECTOR_H__
#include "weather_engine.h"
#include <list>
#include <string>

namespace base_weather{
class CaiyunConnectorImpl:public WeatherConnector{

public:
	CaiyunConnectorImpl();
	virtual ~CaiyunConnectorImpl();

public:
	virtual void Init();
	virtual void Release();
	virtual bool GetWeatherInfo(const std::string& latitude,const std::string& longitude,std::string&temp,
				std::string& current_weather);

	virtual bool GetWeatherInfo(const double latitude,const double longitude,std::string&temp,
				std::string& current_weather);
private:
	std::string  caiyun_weather_host_;
};


}
#endif
