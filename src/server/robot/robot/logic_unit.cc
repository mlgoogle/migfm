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
}

const char* codotodesc(const int32 code){
	switch(code){
	case RAIN:
		return "下雨";
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
}
