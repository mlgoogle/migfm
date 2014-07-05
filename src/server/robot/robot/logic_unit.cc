#include "logic_unit.h"
#include "base/logic_comm.h"
#include "json/json.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
#include "log/mig_log.h"
#include <sstream>
#include <sys/socket.h>
#include <time.h>


int caiyuncode(const char* status){
	if(strcmp(status,"RAIN")==0)
		return RAIN;
	else if(strcmp(status,"CLEAR_DAY") == 0)
		return CLEAR_DAY;
	else if(strcmp(status,"CLEAR_NIGHT") == 0)
		return CLEAR_NIGHT;
	else if(strcmp(status,"PARTLY_CLOUDY_DAY") == 0)
		return PARTLY_CLOUDY_DAY;
	else if(strcmp(status,"PARTLY_CLOUDY_NIGHT") == 0)
		return PARTLY_CLOUDY_NIGHT;
	else if(strcmp(status,"CLOUDY") == 0)
		return CLOUDY;
	else if(strcmp(status,"SLEET") == 0)
		return SLEET;
	else if(strcmp(status,"SNOW") == 0)
		return SNOW;
	else if(strcmp(status,"WIND") == 0)
		return WIND;
	else if(strcmp(status,"FOG") == 0)
		return FOG;
	else
		return UNKONW;
}

const char* codotodesc(const int32 code){
	switch(code){
	case RAIN:
		return "下雨";
	case CLEAR_DAY:
		return "晴朗的白天";
	case CLEAR_NIGHT:
		return "晴朗的夜晚";
	case PARTLY_CLOUDY_DAY:
		return "阴天白天";
	case PARTLY_CLOUDY_NIGHT:
		return "阴天夜晚";
	case CLOUDY:
		return "多云";
	case SLEET:
		return "雨雪";
	case SNOW:
		return "下雪";
	case WIND:
		return "风";
	case FOG:
		return "雾";
	}
}

namespace robot_logic {

bool ResolveJson::ReolveJsonCaiYunJson(const std::string& content, std::string& status,
		std::string& skycon,int32& temp,std::list<double>& dataseries_list){

	bool r = false;
	Json::Reader reader;
	Json::Value  root;
	Json::Value dataseries;
	r = reader.parse(content.c_str(),root);
	if(!r)
		return r;

	if (!root.isMember("status"))
		return false;
	status = root["status"].asString();
	if(status!="ok")
		return false;

	if (!root.isMember("skycon"))
		return false;
	skycon = root["skycon"].asString();

	if (!root.isMember("temp"))
		return false;
	temp = root["temp"].asInt();

	if(!root.isMember("dataseries"))
		return false;
	dataseries = root["dataseries"];

	int dataseries_size = dataseries.size();
	//解析未来一小时状态
	for(int i = 0;i < dataseries_size;i++){
		double datasery = dataseries[0].asDouble();
		dataseries_list.push_back(datasery);
	}
	return true;
}

bool ResolveJson::ReolveJsonBaiduAddress(const std::string& content,std::string& city,
		std::string& district,std::string& province,std::string& street){
	bool r = false;
	int status;
	Json::Reader reader;
	Json::Value  root;
	Json::Value result;
	Json::Value addressComponent;
	r = reader.parse(content.c_str(),root);
	if(!r)
		return r;
	if (!root.isMember("status"))
		return false;
	status = root["status"].asInt();
	if(status!=0)
		return false;
	if(!root.isMember("result"))
		return false;
	result = root["result"];

	if(!result.isMember("addressComponent"))
		return false;
	addressComponent = root["result"]["addressComponent"];

	if (addressComponent.isMember("city"))
		city = addressComponent["city"].asString();

	if (addressComponent.isMember("district"))
		district = addressComponent["district"].asString();

	if (addressComponent.isMember("province"))
		province = addressComponent["province"].asString();

	if (addressComponent.isMember("street"))
		street = addressComponent["street"].asString();

	return true;
}


void LogicUnit::FormateMusicInfo(std::list<std::string>& songinfolist,
			 std::map<std::string,base::MusicInfo>& music_infos){
	  while(songinfolist.size()>0){
		  base::MusicInfo music_info;
		  std::string info = songinfolist.front();
		  music_info.UnserializedJson(info);
		  songinfolist.pop_front();
		  music_infos[music_info.id()] = music_info;
	  }

}

void LogicUnit::CalculateOneHourWeather(std::list<double>& dataseries_list,
		double& weather_result,int& furture_time){
	int rain_flag = 0;
	furture_time = 0;
	while(dataseries_list.size()>0){
		furture_time++;
		double result = dataseries_list.front();
		dataseries_list.pop_front();
		if(result>0){//雨天
			weather_result = result;
			return ;
		}
	}
}

}
