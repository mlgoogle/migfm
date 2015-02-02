/*
 * music_cache_manager.h
 *
 *  Created on: 2015年2月1日
 *      Author: pro
 */

#ifndef MUSIC_CACHE_MANAGER_H_
#define MUSIC_CACHE_MANAGER_H_
#include "logic/base_cache_manager.h"
#include "logic/logic_unit.h"
#include <list>
#include <map>
namespace musicsvc_logic{

class MusicCache:public base_logic::BaseCache{

};
class WholeManager: public base_logic::BaseWholeManager{
public:
	WholeManager();
	~WholeManager();
public:
	void GetCollectList(const int64 uid,MUSICINFO_MAP& music_list);
private:
	//void GetCollectList(const int64 uid);
	void GetMusicListT(const int64 uid,MUSICINFONLIST_MAP& container,
			MUSICINFO_MAP& music_list,void (*redis_get)(const int64,std::list<std::string>&));
private:
	void GetMusicInfo(MUSICINFO_MAP& list);
public:
	MusicCache* GetFindCache(){return this->music_cache_;}
private:
	MusicCache*                   music_cache_;
};


//
class CacheManagerOp{
private:
	static WholeManager              *whole_mgr_;
	static CacheManagerOp            *cache_manager_op_;

	CacheManagerOp(){};
	virtual ~CacheManagerOp(){};
public:
	static WholeManager* GetWholeManager (){
		if (whole_mgr_ == NULL)
			whole_mgr_ = new WholeManager ();

		return whole_mgr_;
	}

	static CacheManagerOp* GetCacheManagerOp(){
		if(cache_manager_op_ == NULL){
			cache_manager_op_ = new CacheManagerOp();
		}
		return cache_manager_op_;
	}

public:
	static void FetchDBMusicURL();
	static void FetchDicMusicBasic();
	static void FetchAvailableMusicInfo();
	//static void FetchDBMusicAbout();

};


}

#endif /* MUSIC_CACHE_MANAGER_H_ */
