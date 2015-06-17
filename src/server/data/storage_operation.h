/*
 * storage_operation.h
 *
 *  Created on: 2015年5月25日
 *      Author: kerry
 *  Time: 下午10:47:48
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */

#ifndef STORAGE_OPERATION_H_
#define STORAGE_OPERATION_H_
#include "data_mem_engine.h"
#include "data_mysql_engine.h"
#include "data_redis_engine.h"
#include "basic/scoped_ptr.h"
#include "config/config.h"
#include "logic/logic_infos.h"
#include "logic/logic_unit.h"

namespace base_logic{

class StorageOperation{
public:
	StorageOperation();
	virtual ~StorageOperation();
public:
	static StorageOperation* Instance();
	static void FreeInstance();
	static StorageOperation* instance_;
public:
	void Init(config::FileConfig* config);
public:
	bool GetUserInfo(const int64 uid,base_logic::UserInfo& info);
	bool BatchGetUserInfo(std::vector<int64>& uid_list,std::map<int64,base_logic::UserInfo>& userinfo);


	bool GetDimensions(const std::string& type,base_logic::Dimensions& dimensions);
	bool GetAllDimension(std::list<base_logic::Dimension>& list);
	bool GetAvailableMusicInfo(std::map<int64,base_logic::MusicInfo>& map);
	bool GetDimensionMusic(const std::string& class_name,const int64 id,
			DIMENSION_MAP& map,DIMENSION_VEC& vec);
	bool GetCollectList(const int64 uid,std::list<std::string>& list);

public:
	static void GetCollectListS(const int64 uid,std::list<std::string>& list);
private:
	scoped_ptr<base_logic::DataMysqlEngne>    mysql_engine_;
	scoped_ptr<base_logic::DataUserMemEngine > mem_engine_;
	scoped_ptr<base_logic::DataMusicReidsEngine> redis_engine_;
};
}



#endif /* STORAGE_OPERATION_H_ */
