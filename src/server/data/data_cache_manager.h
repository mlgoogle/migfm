/*
 * data_cache_manager.h
 *
 *  Created on: 2015年5月19日
 *      Author: pro
 */
#include "logic_infos.h"
#include "logic_unit.h"
#ifndef DATA_CACHE_MANAGER_H_
#define DATA_CACHE_MANAGER_H_

//用于存储各个插件所需要的共享数据

namespace base_logic{

class DataCache{
//用户数据
public:
	USER_INFO_MAP                        userinfo_map_; //用户信息
	MUSICINFO_MAP                        musicinfo_map_;
	MUSICINFONLIST_MAP                   collect_map_;
	MUSICINFONLIST_MAP                   hate_map_;
};


class DataWholeManager{
public:
	DataWholeManager();
	virtual ~DataWholeManager();
public:
	void Init();
public:
	DataCache* GetFindCache(){return this->data_cache_;}
private:
	DataCache*          data_cache_;
};
}






#endif /* DATA_CACHE_MANAGER_H_ */
