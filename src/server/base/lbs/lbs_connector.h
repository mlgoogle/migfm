#ifndef _MIG_FM_LBS_CONNECTOR_H__
#define _MIG_FM_LBS_CONNECTOR_H__

#include "basic/basic_info.h"
#include <list>

namespace base_lbs{
enum StorageImplType{
	IMPL_BAIDU = 0,
	IMPL_AUTONAVI = 1
};

class LbsConnector{
public:
	static LbsConnector* Create(int32 type);
	virtual ~LbsConnector(){}
public:
	virtual void Init(std::list<base::ConnAddr>& addrlist) = 0;//初始化 //因涉及去数据库读取ACCESS_TOKEN
	virtual void Release() = 0;//释放
	virtual bool GeocoderForAddress(const std::string& latitude,const std::string& longitude,
			std::string& city,std::string& district,std::string& province,std::string& street) = 0;//通过坐标获取地址
};

class LbsConnectorEngine{
public:
	LbsConnectorEngine() {}
	virtual ~LbsConnectorEngine() {}

	static void Create(int32 type){
		lbs_connector_engine_ = LbsConnector::Create(base_lbs::IMPL_BAIDU);
	}

	static LbsConnector* GetLbsConnectorEngine (){
		return lbs_connector_engine_;
	}

	static void FreeLbsConnectorEngine  (){
		delete lbs_connector_engine_;
	}

private:
	static LbsConnector   *lbs_connector_engine_;
};

}
#endif
