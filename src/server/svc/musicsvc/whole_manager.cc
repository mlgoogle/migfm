/*
 * whole_manager.cc
 *
 *  Created on: 2015年2月1日
 *      Author: pro
 */

#include "whole_manager.h"
#include "db_comm.h"
#include "basic/template.h"
#include "logic/pub_dic_comm.h"
#include "logic/pub_db_comm.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include <sstream>
namespace musicsvc_logic{

WholeManager* CacheManagerOp::whole_mgr_ = NULL;
CacheManagerOp* CacheManagerOp::cache_manager_op_ = NULL;


WholeManager::WholeManager()
:base_logic::BaseWholeManager(){
	music_cache_ = new MusicCache();
	base_logic::BaseWholeManager::Init();
}

WholeManager::~WholeManager(){
}

void WholeManager::GetDimensionList(const std::string& name,const int64 id,MUSICINFO_MAP& music_list){
	//
	bool r = false;
	MULTI_DIMENSION_MAP multi_dimension_map;
	{
		base_logic::RLockGd lk(lock_);
		if(name=="chl")
			multi_dimension_map = music_cache_->channel_dimension_;
		else if(name=="ms")
			multi_dimension_map = music_cache_->scene_dimension_;
		else if(name=="mm")
			multi_dimension_map = music_cache_->mood_dimension_;

		DIMENSION_MAP dimension_map;
		r = base::MapGet<MULTI_DIMENSION_MAP,MULTI_DIMENSION_MAP::iterator,int64,DIMENSION_MAP>
		(multi_dimension_map,id,dimension_map);

		int i = 0;
		for(DIMENSION_MAP::iterator itr = dimension_map.begin();itr!=dimension_map.end();itr++){
			//获取是10个
			base_logic::MusicInfo info = itr->second;
			music_list[info.id()] = info;
			i++;
			if(i>10)
				break;
		}

	}
	GetMusicInfo(music_list);
}


void WholeManager::GetCollectList(const int64 uid,MUSICINFO_MAP& music_list){
	GetMusicListT(uid,music_cache_->collect_map_,music_list,basic_logic::PubDicComm::GetColllectList);
	GetMusicInfo(music_list);

}

void WholeManager::GetMusicInfo(MUSICINFO_MAP& list){
//
	base_logic::RLockGd lk(lock_);
	bool r = false;
	MUSICINFO_MAP::iterator it  = list.begin();
	for(;it!=list.end();it++){
		base_logic::MusicInfo tmusicinfo = it->second;
		base_logic::MusicInfo rmusicinfo;
		//读取信息
		r = base::MapGet<MUSICINFO_MAP,MUSICINFO_MAP::iterator,
				int64,base_logic::MusicInfo >(music_cache_->music_info_map_,tmusicinfo.id(),rmusicinfo);
		if(r)
			tmusicinfo.BaseCopy(rmusicinfo);
	}
}

void WholeManager::GetMusicListT(const int64 uid,MUSICINFONLIST_MAP& container,
		MUSICINFO_MAP& music_list,void (*redis_get)(const int64,std::list<std::string>&)){
	bool r = false;
	std::list<std::string> list;
	{
		base_logic::RLockGd lk(lock_);
		//加锁
		r = base::MapGet<MUSICINFONLIST_MAP,MUSICINFONLIST_MAP::iterator,
				int64,MUSICINFO_MAP >
		(container,uid,music_list);
	}

	if(!r)//读取
		redis_get(uid,list);
	if(list.size()<=0)//
			return ;
		//写入存储
	while(list.size()>0){//
		std::string str = list.front();
		list.pop_front();
		base_logic::MusicInfo info;
		info.JsonSeralize(str);
		music_list[info.id()] = info;
		//music_list[atoll(str_songid.c_str())] = atoll(str_songid.c_str());
		//music_list.push_back(atoll(str_songid.c_str()));
	}
	{
		base_logic::WLockGd lk(lock_);
		//加锁
		r = base::MapAdd<MUSICINFONLIST_MAP,int64,MUSICINFO_MAP >
		(container,uid,music_list);
	}
}

/*
void WholeManager::GetCollectList(const int64 uid,
	std::map<int64,int64>& collect_list){
	//检测是否存在,不存在则读取redis
	bool r = false;
	std::list<std::string> list;
	bool r =  false;
	{
		logic::RLockGd lk(lock_);
		//加锁
		r = base::MapGet<std::map<int64,std::map<int64,int64>>,int64,std::map<int64,int64> >
		(music_cache_->collect_map_,uid,collect_list);
	}
	if(!r)//读取
		basic_logic::PubDicComm::GetColllectList(uid,list);
	if(!r)//无红心歌单
		return ;
	//写入存储
	while(list.size()>0){//
		std::string str_songid = list.front();
		list.pop_front();
		collect_list.push_back(atoll(str_songid.c_str()));
	}
	{
		logic::WLockGd lk(lock_);
		//加锁
		r = base::MapGet<std::map<int64,std::map<int64,int64>>,int64,std::map<int64,int64> >
			MapAdd(music_cache_->collect_map_,uid,collect_list);
	}
}*/

void CacheManagerOp::FetchDicMusicBasic(){
//读取基础信息
}

void CacheManagerOp::FetchDBMusicURL(){
//读取所有URL信息
	basic_logic::PubDBComm::GetMusicURL(whole_mgr_->GetFindCache()->music_info_map_);
}


void CacheManagerOp::FetchAvailableMusicInfo(){
	basic_logic::PubDBComm::GetAvailableMusicInfo(whole_mgr_->GetFindCache()->music_info_map_);
}

void CacheManagerOp::FetchDimensionMusic(){
//读取维度信息
	std::list<base_logic::Dimension> list;
	musicsvc_logic::DBComm::GetAllDimension(list);
//读取每个维度的歌曲
	while(list.size()>0){
		base_logic::Dimension dimension = list.front();
		list.pop_front();
		FectchDimensionMusic(dimension);
	}

}



void CacheManagerOp::FectchDimensionMusic(base_logic::Dimension& dimension){
	// 获取所有歌曲
	std::list<std::string> list;
	std::stringstream os;
	os<<dimension.class_name()<<"_r"<<dimension.id();
	std::string key;
	key = os.str();
	DIMENSION_MAP  dimension_map;
	basic_logic::PubDicComm::GetDimensionMusicList(key,list);
	while(list.size()>0){
		std::string str = list.front();
		list.pop_front();
		base_logic::MusicInfo info;
		info.set_id(atoll(str.c_str()));
		info.set_class_name(dimension.class_name());
		info.set_class(dimension.id());
		//添加歌曲
		dimension_map[info.id()] = info;
	}

	//根据维度添加缓存
	if(dimension.class_name()=="chl")
		whole_mgr_->GetFindCache()->channel_dimension_[dimension.id()] = dimension_map;
	else if(dimension.class_name()=="ms")
		whole_mgr_->GetFindCache()->mood_dimension_[dimension.id()] = dimension_map;
	else if(dimension.class_name()=="mm")
		whole_mgr_->GetFindCache()->scene_dimension_[dimension.id()] = dimension_map;

}

}




