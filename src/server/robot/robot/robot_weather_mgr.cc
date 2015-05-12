#include "robot_weather_mgr.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "http/http_method.h"
#include "logic/logic_comm.h"
#include "base/comm_head.h"
#include "basic/base64.h"
#include <sstream>

namespace robot_logic{

RobotWeatherMgr::RobotWeatherMgr()
:get_user_from_tick_(0)
,get_user_count_tick_(10){

}

RobotWeatherMgr::~RobotWeatherMgr(){

}

bool RobotWeatherMgr::OnPullWeatherInfo(void){
	bool r = false;
	std::list<robot_base::UserLbsInfo> user_lbs_list;
	std::list<struct HandleSongInfo*> list;
	r = robot_storage::DBComm::GetUsersLBSPos(user_lbs_list,get_user_from_tick_,get_user_count_tick_);
	if(!r)
		return false;
	while(user_lbs_list.size()>0){
		robot_base::UserLbsInfo user_lbs = user_lbs_list.front();
		user_lbs_list.pop_front();
		int64 songid;
		std::string message;
		r = PackageWeatherInfo(user_lbs.uid(),user_lbs.latitude(),user_lbs.longitude(),songid,message);
		//批量发给咪呦助手
		if(r){
			struct HandleSongInfo* handlse_song = new struct HandleSongInfo;
			handlse_song->uid = user_lbs.uid();
			handlse_song->songid = songid;
			memset(&handlse_song->message,'\0',MESSAGE_LEN);
			snprintf(handlse_song->message, arraysize(handlse_song->message),
					message.c_str());
			LOG_DEBUG2("uid :%d songid %d message:%s",handlse_song->uid,handlse_song->songid,
					handlse_song->message);
			list.push_back(handlse_song);
		}

	}
	if(list.size()<=0)
		return false;
	//发送给咪呦助手
	CacheManagerOp::GetRobotCacheMgr()->SendAssistantHandlseSong(10000,list);
	return true;
}

bool RobotWeatherMgr::PackageWeatherInfo(const int64 uid,const std::string& latitude,
		const std::string& longitude,int64& songid,std::string& message){
	bool r = false;
	std::stringstream os;
	int32 temp;
	int32 curret_weather;
	int32 future_weather;
	int32 future_time;
	std::string city;
	std::string district;
	std::string province;
	std::string street;
	//int64 songid;
	std::string artist;
	std::string title;

	r = RequestCaiYunWeather(latitude,longitude,temp,curret_weather,future_weather,future_time);
	if(!r)
		return r;
	//存储用户及天气
	CacheManagerOp::GetRobotCacheMgr()->SetUserInfo(10000,uid,curret_weather,future_weather,future_time);

	//缓存中获取地址，如果没有则从百度中获取地址
	r = CacheManagerOp::GetRobotCacheMgr()->GetUserAddressInfo(10000,uid,city,district,province,street);
	//获取地址信息
	if(!r){
		r = RequestBaiduAddress(latitude,longitude,city,district,province,street);
		CacheManagerOp::GetRobotCacheMgr()->SetUserAddressInfo(10000,uid,city,district,province,street);
	}

	//判断用户今日是否已经推送过
	r =CacheManagerOp::GetRobotCacheMgr()->IsPushMessageDay(10000,uid);
	if(r){
		//获取歌曲
		GetWeatherMusic(curret_weather,songid,artist,title);
		//拼装文字
		const char* current_weather_desc = codotodesc(curret_weather);
		os<<"一个"<<temp<<"°"<<current_weather_desc<<"的天气,助理小哟为你带来一首"<<artist<<"的"<<title<<".(位于"<<province
			<<city<<district<<street<<")";
		message = os.str();
		CacheManagerOp::GetRobotCacheMgr()->SetUserPushMessageDay(10000,uid);
		r = true;
	}else{// 不再推送
		r = false;
	}

	return r;

}

//http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD
bool RobotWeatherMgr::RequestCaiYunWeather(const std::string& latitude,const std::string& longitude,
		int32& temp,int32& curret_weather,int32& future_weather,int32& futrue_time){
	//http://rain.swarma.net/fcgi-bin/v1/api.py?lonlat=116.5754,39.8296&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD
	bool r = false;
	std::string host_api = "http://rain.swarma.net/fcgi-bin/v1/api.py";
	std::stringstream os;
	os<<"?lonlat="<<latitude<<","<<longitude<<"&format=json&product=minutes_prec&token=AAEHD3736dKDGEDKUEHD";
	std::string request_url;
	std::string content;
	request_url.append(host_api);
	request_url.append(os.str());

	http::HttpMethodGet caiyun_http(request_url);
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
	curret_weather = caiyuncode(skycon.c_str());
	//计算未来一小时天气状况
	double d_future_weather = 0;
	robot_logic::LogicUnit::CalculateOneHourWeather(dataseries_list,d_future_weather,futrue_time);
	if(d_future_weather>0)//雨天
		future_weather = 0;
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
	http::HttpMethodGet baidu_http(request_url);
	r = baidu_http.Get();
	if(!r)
		return r;
	r = baidu_http.GetContent(content);
	if(!r)
		return r;
	r = robot_logic::ResolveJson::ReolveJsonBaiduAddress(content,city,district,province,street);
	return r;
}

bool RobotWeatherMgr::GetWeatherMusic(const int type_id,int64& songid,std::string& singer,std::string& song){
	std::list<int64> list;
	std::list<std::string> songinfolist;
	std::map<std::string,base::MusicInfo>  musicinfomap;
	std::stringstream os;
	std::string type = "chl";
	int mode_id = 1;
	os<<type<<"_r"<<mode_id;
	bool r = CacheManagerOp::GetRobotCacheMgr()->GetModeRadomSong(10000,
			type,mode_id,1,list); //获取随机ID

	//获取音乐信息
	r = robot_storage::RedisComm::GetBatchMusicInfos(os.str(),list,songinfolist);
	//转化音乐信息
	robot_logic::LogicUnit::FormateMusicInfo(songinfolist,musicinfomap);
	//取第一个
	std::map<std::string,base::MusicInfo>::iterator itr = musicinfomap.begin();
	if(itr!=musicinfomap.end()){
		std::string b64title;
		std::string b64singer;
		base::MusicInfo musicinfo = itr->second;
		songid = atoll(musicinfo.id().c_str());
		Base64Decode(musicinfo.title(),&song);
		Base64Decode(musicinfo.artist(),&singer);
	}
	return true;
}



}
