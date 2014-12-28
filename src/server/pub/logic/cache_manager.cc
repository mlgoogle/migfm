/*
 * cache_manager.cc
 *
 *  Created on: 2014年12月11日
 *      Author: kerry
 */
#include "cache_manager.h"
#include "pub_db_comm.h"
#include "logic_unit.h"
#include <string>
namespace base_logic{

WholeManager* WholeManager::whole_manager_ = NULL;

int64 WholeManager::dimensions_name(const std::string& alias,
		const int64 id,std::string& name){
	std::string empty;
	base_logic::Dimensions  dimensions;

	std::map<std::string,base_logic::Dimensions>::iterator it = dimensions_.find(alias);
	if(it!=dimensions_.end()){
		dimensions = it->second;
		it->second.dimension_name(id,name);
		return it->second.id() + id;
	}else{
		name = UNKONW_DIMENSIONS;
		return UNKONW_DIMENSIONS_ID;
	}
}

void WholeManager::Init(){
	//创建三个类别的容器
	CreateDimensions(10000,"ms",MOOD_NAME);
	CreateDimensions(20000,"mm",SCENE_NAME);
	CreateDimensions(30000,"chl",CHANNEL_NAME);
}

void  WholeManager::CreateDimensions(const int64 id,
		const std::string& type,const std::string& name){
	base_logic::Dimensions dimensions(id,name,type);
	basic_logic::PubDBComm::GetDimensions(type,dimensions);
	dimensions_[type] = dimensions;
}


}



