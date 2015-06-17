/*
 * data_redis_engine.h
 *
 *  Created on: 2015年5月31日
 *      Author: pro
 */

#ifndef DATA_REDIS_ENGINE_H_
#define DATA_REDIS_ENGINE_H_

#include "storage_controller_engine.h"
#include "logic/logic_infos.h"
#include "logic/logic_unit.h"
#include "storage/storage.h"
#include "basic/scoped_ptr.h"

namespace base_logic{

class DataBaseRedisEngine{
public:
	DataBaseRedisEngine(){};
	virtual ~DataBaseRedisEngine(){};
public:
	scoped_ptr<base_logic::DataControllerEngine> redis_engine_;
};

class DataMusicReidsEngine:public DataBaseRedisEngine{
public:
	DataMusicReidsEngine();
	virtual ~DataMusicReidsEngine();
public:
	bool GetDimensionMusic(const std::string& class_name,const int64 id,
			DIMENSION_MAP& map,DIMENSION_VEC& vec);

	bool GetCollectList(const int64 uid,std::list<std::string>& list);
};
}



#endif /* DATA_REDIS_ENGINE_H_ */
