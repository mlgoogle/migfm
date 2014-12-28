#include "caiyun_weather_engine.h"
#include "weather_logic_unit.h"
#include "http/http_method.h"
#include "logic/logic_comm.h"
#include <sstream>
namespace base_weather{

CaiyunConnectorImpl::CaiyunConnectorImpl(){

}

CaiyunConnectorImpl::~CaiyunConnectorImpl(){

}

void CaiyunConnectorImpl::Init(){
	caiyun_weather_host_ = "http://caiyunapp.com/fcgi-bin/v1/api.py";
}

void CaiyunConnectorImpl::Release(){

}

//http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD
bool CaiyunConnectorImpl::GetWeatherInfo(const double latitude,const double longitude,std::string&temp,
		std::string& current_weather){
	bool r = false;
	std::stringstream os;
	std::string content;
	std::string  status;
	int32 caiyun_temp_value;
	int i = 0;
	caiyun_temp_value = 0;
	os<<caiyun_weather_host_<<"?lonlat="<<longitude<<","<<latitude<<"&format=json&product=minutes_prec&token=ZG9uZ3hpYW5nIG11c2ljIGFwaQ==";
	//os<<"http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD";
	std::string url = os.str();
	do{
		i++;
		if(i>3)
			break;
		//LOG_DEBUG2("%s",os.str().c_str());
		std::string url = os.str();
		http::HttpMethodGet caiyun_http(url);
		r = caiyun_http.Get();
		if(!r)
			continue;
		r = caiyun_http.GetContent(content);
		if(!r)
			continue;

	r = base_weather::ResolveJson::ReolveJsonCaiYunWeather(content,status,current_weather,caiyun_temp_value);
	if(r)
		break;
	if(!r)
		continue;
	if(current_weather.empty()||caiyun_temp_value==0)
		continue;
	}while(true);
	os.str("");
	os<<caiyun_temp_value<<"℃";
	temp = os.str();
	return true;
}

//http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD
bool CaiyunConnectorImpl::GetWeatherInfo(const std::string& latitude,const std::string& longitude,std::string&temp,
		std::string& current_weather){
	bool r = false;
	std::stringstream os;
	std::string content;
	std::string  status;
	int32 caiyun_temp_value;
	int i = 0;
	caiyun_temp_value = 0;
	os<<caiyun_weather_host_<<"?lonlat="<<longitude<<","<<latitude<<"&format=json&product=minutes_prec&token=ZG9uZ3hpYW5nIG11c2ljIGFwaQ==";
	//os<<"http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD";
	std::string url = os.str();
	do{
		i++;
		if(i>3)
			break;
		//LOG_DEBUG2("%s",os.str().c_str());
		std::string url = os.str();
		http::HttpMethodGet caiyun_http(url);
		r = caiyun_http.Get();
		if(!r)
			continue;
		r = caiyun_http.GetContent(content);
		if(!r)
			continue;

	r = base_weather::ResolveJson::ReolveJsonCaiYunWeather(content,status,current_weather,caiyun_temp_value);
	if(r)
		break;
	if(!r)
		continue;
	if(current_weather.empty()||caiyun_temp_value==0)
		continue;
	}while(true);
	os.str("");
	os<<caiyun_temp_value<<"℃";
	temp = os.str();
	return true;
}


}
