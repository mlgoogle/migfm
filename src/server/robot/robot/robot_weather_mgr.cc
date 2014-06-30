#include "robot_weather_mgr.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "http_method.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
#include "basic/base64.h"
#include <sstream>

namespace robot_logic{

RobotWeatherMgr::RobotWeatherMgr(){

}

RobotWeatherMgr::~RobotWeatherMgr(){

}

bool RobotWeatherMgr::OnPullWeatherInfo(void){
	bool r = false;
	std::list<robot_base::UserLbsInfo> user_lbs_list;
	r = robot_storage::DBComm::GetUsersLBSPos(user_lbs_list);
	if(!r)
		return false;

	while(user_lbs_list.size()>0){
		robot_base::UserLbsInfo user_lbs = user_lbs_list.front();
		user_lbs_list.pop_front();
		PackageWeatherInfo(user_lbs.uid(),user_lbs.latitude(),user_lbs.longitude());
		//RequestCaiYunWeather(user_lbs.latitude(),user_lbs.longitude());
		//获取地区
	}
	return true;
}

bool RobotWeatherMgr::PackageWeatherInfo(const int64 uid,const std::string& latitude,const std::string& longitude){
	bool r = false;
	std::stringstream os;
	int32 temp;
	int32 curret_weather;
	int32 future_weather;
	std::string city;
	std::string district;
	std::string province;
	std::string street;

	r = RequestCaiYunWeather(latitude,longitude,temp,curret_weather,future_weather);
	if(!r)
		return r;
	//获取地址信息
	r = RequestBaiduAddress(latitude,longitude,city,district,province,street);
	//拼装文字
	const char* current_weather_desc = codotodesc(curret_weather);
	os<<"一个"<<temp<<"°的"<<current_weather_desc<<"天,适合听窦唯的无地自容.(位于"<<province
			<<city<<district<<street<<")";
	LOG_DEBUG2("%s",os.str().c_str());
	return true;

}

//http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD
bool RobotWeatherMgr::RequestCaiYunWeather(const std::string& latitude,const std::string& longitude,
		int32& temp,int32& curret_weather,int32& future_weather){
	//http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD
	bool r = false;
	std::string host_api = "http://rain.swarma.net/fcgi-bin/v1/api.py";
	std::stringstream os;
	os<<"?lonlat="<<latitude<<","<<longitude<<"&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD";
	std::string request_url;
	std::string content;
	request_url.append(host_api);
	request_url.append(os.str());

	robot_logic::HttpMethodGet caiyun_http(request_url);
	r = caiyun_http.Get();
	if(!r)
		return r;
	r = caiyun_http.GetContent(content);
	if(!r)
		return r;
	//LOG_DEBUG2("content %s",content.c_str());
	std::string  status;
	std::string skycon;
	std::list<double> dataseries_list;
	r = robot_logic::ResolveJson::ReolveJsonCaiYunJson(content,status,skycon,temp,dataseries_list);
	if(!r)
		return r;
	LOG_DEBUG2("skycon: %s",skycon.c_str());
	//计算未来一小时天气状态
	curret_weather = 0;
	future_weather = 1;
	return true;
}

//http://api.map.baidu.com/geocoder/v2/?ak=esMsm0k6HDMDOFLxRqtRoWs7&location=30.267600,120.181000&output=json&pois=0
bool RobotWeatherMgr::RequestBaiduAddress(const std::string& latitude,const std::string& longitude,
		std::string& city,std::string& district,std::string& province,std::string& street){
	bool r = false;
	std::string host_api = "http://api.map.baidu.com/";
	std::stringstream os;
	os<<"geocoder/v2/?ak=esMsm0k6HDMDOFLxRqtRoWs7&location="<<longitude<<","<<latitude<<"&output=json&pois=0";
	std::string request_url;
	std::string content;
	request_url.append(host_api);
	request_url.append(os.str());
	robot_logic::HttpMethodGet baidu_http(request_url);
	r = baidu_http.Get();
	if(!r)
		return r;
	r = baidu_http.GetContent(content);
	if(!r)
		return r;
	r = robot_logic::ResolveJson::ReolveJsonBaiduAddress(content,city,district,province,street);
	return r;
}

}
