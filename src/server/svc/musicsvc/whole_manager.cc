/*
 * whole_manager.cc
 *
 *  Created on: 2015年2月1日
 *      Author: pro
 */

#include "whole_manager.h"
#include "basic/template.h"
#include "logic/pub_dic_comm.h"
#include "logic/pub_db_comm.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
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

void CacheManagerOp::FetchDBMusicURL(){
//读取所有URL信息
	basic_logic::PubDBComm::GetMusicURL(whole_mgr_->GetFindCache()->music_info_map_);
}


void CacheManagerOp::FetchAvailableMusicInfo(){
	basic_logic::PubDBComm::GetAvailableMusicInfo(whole_mgr_->GetFindCache()->music_info_map_);
}

void CacheManagerOp::FetchDicMusicBasic(){
//读取基础信息
}

}




