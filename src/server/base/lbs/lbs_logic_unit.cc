#include "lbs_logic_unit.h"
#include "json/json.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
namespace base_lbs{

bool ResolveJson::ReolveJsonCaiYunWeather(const std::string& content,std::string& city,
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

}
