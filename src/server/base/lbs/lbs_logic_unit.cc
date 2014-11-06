#include "lbs_logic_unit.h"
#include "json/json.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
namespace base_lbs{

bool ResolveJson::ReolveJsonBaiduIPToAddress(const std::string& content,std::string& city,
			std::string& district,std::string& province,std::string& street,std::string& latitude,
			std::string& longitude){
	bool r = false;
	int status;
	Json::Reader reader;
	Json::Value  root;
	Json::Value result;
	Json::Value addressComponent;
	Json::Value address_detail;
	Json::Value address_point;
	r = reader.parse(content.c_str(),root);
	if(!r)
		return r;
	if (!root.isMember("status"))
		return false;
	status = root["status"].asInt();
	if(status!=0)
		return false;
	if(!root.isMember("content"))
		return false;

	addressComponent = root["content"];

	if(!addressComponent.isMember("address_detail"))
		return false;

	address_detail = addressComponent["address_detail"];

	if (address_detail.isMember("city"))
		city = address_detail["city"].asString();

	if (address_detail.isMember("district"))
		district = address_detail["district"].asString();

	if (address_detail.isMember("province"))
		province = address_detail["province"].asString();

	if (address_detail.isMember("street"))
		street = address_detail["street"].asString();

	if(!addressComponent.isMember("point"))
		return false;

	address_point = addressComponent["point"];

	if (address_point.isMember("x"))
		longitude = address_point["x"].asString();

	if (address_point.isMember("y"))
		latitude = address_point["y"].asString();

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

bool ResolveJson::ReolveJsonBaiduAddressForGeocoder(const std::string& content,double& latitude,
		double& longitude){
	bool r = false;
	int status;
	Json::Reader reader;
	Json::Value  root;
	Json::Value result;
	Json::Value location;
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

	if(!result.isMember("location"))
		return false;
	location = root["result"]["location"];

	if (location.isMember("lng"))
		longitude = location["lng"].asDouble();

	if (location.isMember("lat"))
		latitude = location["lat"].asDouble();
	return true;
}

}
