#ifndef _MIG_FM_LBS_BAIDU_CONNECTOR_H__
#define _MIG_FM_LBS_BAIDU_CONNECTOR_H__
#include "lbs_connector.h"
#include "basic_lbs_info.h"
#include "basic/basic_info.h"
#include <list>
#include <string>

namespace base_lbs{
class LbsBaiduConnectorImpl:public LbsConnector{

public:
	LbsBaiduConnectorImpl();
	virtual ~LbsBaiduConnectorImpl();

public:
	virtual void Init(std::list<base::ConnAddr>& addrlist);
	virtual void Release();
	virtual bool GeocoderForAddress(const std::string& latitude,const std::string& longitude,
			std::string& city,std::string& district,std::string& province,std::string& street);//通过坐标获取地址
private:
	bool GetIdleAccessToken(base_lbs::BaiDuAccessKey& baidu_access_key);
	bool RequestBaiduUrl(const std::string& url,std::string& content);
private:
	std::list<base_lbs::BaiDuAccessKey>    access_key_list_;
	std::string                       baidu_map_host_;
};
}
#endif
