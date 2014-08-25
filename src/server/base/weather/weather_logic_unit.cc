#include "weather_logic_unit.h"
#include "json/json.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
namespace base_weather{

bool ResolveJson::ReolveJsonCaiYunWeather(const std::string& content,std::string& status,
		std::string& skycon,int32& temp){

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

	return true;
}

}
