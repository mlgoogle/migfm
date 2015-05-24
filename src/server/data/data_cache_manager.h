/*
 * data_cache_manager.h
 *
 *  Created on: 2015年5月19日
 *      Author: pro
 */

#ifndef DATA_CACHE_MANAGER_H_
#define DATA_CACHE_MANAGER_H_

#include "logic/logic_infos.h"
#include "logic/logic_unit.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"


//用于存储各个插件所需要的共享数据
#include "config/config.h"
namespace base_logic{



class DataEngine{
public:
	virtual bool SetUserInfo(const int64 uid,base_logic::UserInfo& info) = 0;

	virtual bool DelUserInfo(const int64 uid) = 0;
};


class DateEngineImpl:public DataEngine{
public:
	bool SetUserInfo(const int64 uid,base_logic::UserInfo& info);

	bool DelUserInfo(const int64 uid);
};


class DataCache{
//用户数据
public:
	USER_INFO_MAP                        userinfo_map_; //用户信息
	MUSICINFO_MAP                        musicinfo_map_;
	MUSICINFONLIST_MAP                   collect_map_;
	MUSICINFONLIST_MAP                   hate_map_;
};

__attribute__((visibility("default")))
class DataWholeManager{
public:
	DataWholeManager();
	virtual ~DataWholeManager();

public:
	void Init(std::list<base::ConnAddr>& addrlist);
public:
	bool SetUserInfo(const int64 uid,base_logic::UserInfo& info);
	bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
	bool DelUserInfo(const int64 uid);
private:
	void Init();


public:
	DataCache* GetFindCache(){return this->data_cache_;}
private:
	struct threadrw_t*            lock_;
	DataCache*                    data_cache_;
};



class DataCacheEngine{
private:
	static DataWholeManager              *whole_mgr_;
	static DataCacheEngine               *data_cache_engine_;

	DataCacheEngine(){};
	virtual ~DataCacheEngine(){};
public:
	__attribute__((visibility("default")))
	static DataWholeManager* GetWholeManager (){
		if (whole_mgr_ == NULL)
			whole_mgr_ = new DataWholeManager ();

		return whole_mgr_;
	}

	static DataCacheEngine* GetDataCacheEngine(){
		if(data_cache_engine_ == NULL){
			data_cache_engine_ = new DataCacheEngine();
		}
		return data_cache_engine_;
	}
};


}


extern "C" base_logic::DataEngine *GetDateEngine(void);





#endif /* DATA_CACHE_MANAGER_H_ */
