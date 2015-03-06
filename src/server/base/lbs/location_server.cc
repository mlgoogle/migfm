/*
 * location_server.cc
 *
 *  Created on: 2015年3月6日
 *      Author: pro
 */

#include "location_server.h"
#include "lbs_connector.h"
#include "lbs_logic_unit.h"
#include "lbs_dic_comm.h"
#include "basic_lbs_info.h"
#include "config/config.h"

namespace base_lbs{

void LocationServer::Init(std::list<base::ConnAddr>& db_addrlist,
		std::list<base::ConnAddr>& mem_addrlist){
	base_lbs::LbsConnectorEngine::Create(base_lbs::IMPL_BAIDU);
	base_lbs::LbsConnector* engine = base_lbs::LbsConnectorEngine::GetLbsConnectorEngine();
	engine->Init(db_addrlist);

	base_lbs::MemComm::Init(mem_addrlist);
}

void LocationServer::Dest(){
	base_lbs::LbsConnectorEngine::FreeLbsConnectorEngine();
	base_lbs::MemComm::Dest();
}

bool LocationServer::IPtoAddress(const std::string& host,double& latitude,double& longitude,
		std::string& city,std::string& district,std::string& province,std::string& street){
	return base_lbs::LbsConnectorEngine::GetLbsConnectorEngine()->IPtoAddress(host,latitude,longitude,
			city,district,province,street);
}

bool LocationServer::GeocoderForAddress(const double latitude,const double longitude,
		std::string& city,std::string& district,std::string& province,std::string& street){

	//获取缓存
	base_lbs::LBSInfos location;
	char* data = NULL;
	size_t len = 0;
	bool r = base_lbs::MemComm::GetLocation(latitude,longitude,&data,&len);
	if(!r){//请求百度
		base_lbs::LbsConnectorEngine::GetLbsConnectorEngine()->GeocoderForAddress(latitude,longitude,city,district,province,street);
		//写入缓存
		std::string json;
		location.set_city(city);
		location.set_district(district);
		location.set_province(province);
		location.set_street(street);
		location.set_current_time(time(NULL));
		location.JsonSerializer(&json);
		base_lbs::MemComm::SetLocation(latitude,longitude,json.c_str(),json.length());
	}else{
		std::string json;
		json.assign(data,len);
		location.JsonDeserialize(&json);
		city = location.city();
		district = location.district();
		province = location.province();
		street = location.street();
		//检测 如果大于指定时间则删除
		if((time(NULL) - location.current_time())>(60 * 60 * 24 * 4))
			base_lbs::MemComm::DelLocation(latitude,longitude);
	}
	return true;
}

}



