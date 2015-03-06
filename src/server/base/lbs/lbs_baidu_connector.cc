#include "lbs_baidu_connector.h"
#include "lbs_logic_unit.h"
#include "lbs_db_comm.h"
#include "http/http_method.h"
#include "basic/basic_util.h"
#include <sstream>
namespace base_lbs{

LbsBaiduConnectorImpl::LbsBaiduConnectorImpl(){
}

LbsBaiduConnectorImpl::~LbsBaiduConnectorImpl(){

}

void LbsBaiduConnectorImpl::Init(std::list<base::ConnAddr>& addrlist){
	baidu_map_host_ = "http://api.map.baidu.com/geocoder/v2/?output=json&pois=0&";
	baidu_local_host_ = "http://api.map.baidu.com/location/";
//初始化数据库
	base_lbs::LBSDBComm::Init(addrlist);

	//读取ACCESS_TOKEN
	bool r = base_lbs::LBSDBComm::GetBaiduAcessKey(10000,this->access_key_list_);
	if(!r){
		assert(0);
		return ;
	}

}

void LbsBaiduConnectorImpl::Release(){

}

//http://api.map.baidu.com/location/ip?ak=esMsm0k6HDMDOFLxRqtRoWs7&ip=124.160.208.127&coor=bd09ll
bool LbsBaiduConnectorImpl::IPtoAddress(const std::string& host,double& latitude,double& longitude,
				std::string& city,std::string& district,std::string& province,std::string& street){
	//向百度请求
	bool r = false;
	std::stringstream os;
	base_lbs::BaiDuAccessKey baidu_access_key;
	std::string content;
	int i = 0;

	do{
		r = GetIdleAccessToken(baidu_access_key);
		if(!r)
			return false;
		os<<baidu_local_host_<<"ip?"<<"ak="<<baidu_access_key.access_token()<<"&ip="<<host<<"&coor=bd09ll";
		r = RequestBaiduUrl(os.str(),content);
		if(r)
			break;
		if(i>=3)
			return false;
	}while(true);
	baidu_access_key.add_count();
	r = base_lbs::ResolveJson::ReolveJsonBaiduIPToAddress(content,city,district,province,street,latitude,
			longitude);

	return r;
}

//http://api.map.baidu.com/geocoder/v2/?output=json&pois=0&ak=esMsm0k6HDMDOFLxRqtRoWs7&location=30.267600,120.181000
bool LbsBaiduConnectorImpl::GeocoderForAddress(const double latitude,const double longitude,
			std::string& city,std::string& district,std::string& province,std::string& street){
	//向百度请求
	bool r = false;
	std::stringstream os;
	base_lbs::BaiDuAccessKey baidu_access_key;
	std::string content;
	int i = 0;

	do{
		r = GetIdleAccessToken(baidu_access_key);
		if(!r)
			return false;
		os<<baidu_map_host_<<"ak="<<baidu_access_key.access_token()<<"&location="<<latitude<<","<<longitude;
		r = RequestBaiduUrl(os.str(),content);
		if(r)
			break;
		if(i>=3)
			return false;
	}while(true);
	baidu_access_key.add_count();
	r = base_lbs::ResolveJson::ReolveJsonBaiduAddress(content,city,district,province,street);
	return r;
}

//http://api.map.baidu.com/geocoder/v2/?output=json&ak=esMsm0k6HDMDOFLxRqtRoWs7&address=江干下沙下沙东沙商业中心C座
bool LbsBaiduConnectorImpl::AddressForGeocoder(const std::string& address,double& latitude,
		double& longitude){
	//向百度请求
	bool r = false;
	std::stringstream os;
	base_lbs::BaiDuAccessKey baidu_access_key;
	std::string content;
	std::string urlcode;
	int i = 0;

	do{
		r = GetIdleAccessToken(baidu_access_key);
		if(!r)
			return false;
		r = base::BasicUtil::UrlEncode(address,urlcode);
		if(r)
			os<<baidu_map_host_<<"ak="<<baidu_access_key.access_token()<<"&address="<<urlcode;
		else
			os<<baidu_map_host_<<"ak="<<baidu_access_key.access_token()<<"&address="<<address;
		r = RequestBaiduUrl(os.str(),content);
		baidu_access_key.add_count();
		if(r)
			break;
		if(i>=3)
			return false;
		i++;
		os.str("");
	}while(true);
	if(!content.empty())
		r = base_lbs::ResolveJson::ReolveJsonBaiduAddressForGeocoder(content,latitude,longitude);
	return r;

}

bool LbsBaiduConnectorImpl::RequestBaiduUrl(const std::string& url,std::string& content){
	http::HttpMethodGet baidu_http(url);
	bool r = baidu_http.Get();
	if(!r)
		return r;
	r = baidu_http.GetContent(content);
	if(!r)
		return r;
	return true;
}


bool LbsBaiduConnectorImpl::GetIdleAccessToken(base_lbs::BaiDuAccessKey& baidu_access_key){
	if(this->access_key_list_.size()<=0)
		return false;
	std::list<base_lbs::BaiDuAccessKey>::iterator it = this->access_key_list_.begin();
	baidu_access_key = (*it);
	for(;it!=this->access_key_list_.end();it++){
		base_lbs::BaiDuAccessKey temp_baidu_access_key = (*it);
		if(temp_baidu_access_key.count()<baidu_access_key.count()){
			baidu_access_key = temp_baidu_access_key;
		}
	}
	return true;
}



}
