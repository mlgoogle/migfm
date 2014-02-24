#ifndef _MASTER_PLUGIN_MUSIC_MGR_ENGINE_H__
#define _MASTER_PLUGIN_MUSIC_MGR_ENGINE_H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "get_song.h"
#include "basic/basic_info.h"
#include "basic/basictypes.h"
#include <list>
#include <vector>
#include <map>
namespace music_logic{

class ChannelCache{
public:
	base::ChannelInfo                channel_info_;
	std::list<base::MusicInfo>       channel_music_infos_;
	time_t                           current_time;
};

class MusicCacheManager{

public:
	MusicCacheManager();
	virtual ~MusicCacheManager();
	bool InitMusicChannel();
	bool AddMusicChannelInfos(int channel,std::list<base::MusicInfo>& list);
	bool GetMusicChannelInfos(int channel,std::string& json_content,
		                      const int flag =0,const int cur_num = 3);//0:html5 1:ios

	bool GetMusicCahnelTypeInfos(int channel,const std::string& uid,const int nun,
		                         std::stringstream& os);

	int  GetMusicCHannelNum(int channel);
	time_t GetMusicTime(int channel);
	void IsLessMuciChannelInfos(const std::string& channel, int num);
	void IsTimeMusiChannelInfos(const std::string& channel);
	bool RequestDoubanMusicInfos(const std::string& channel,
		                         std::string& content);

	bool PutJsonMusicChannel(int channel,std::string& content);



	bool GetMusicChannel(std::string& num,std::string& content);
private:
	ChannelCache *GetChannelCache(int32 channel);
private:
	threadrw_t *                                  cache_mgr_lock_;
	std::vector<base::ChannelInfo>                channel_mode_;
	int                                           channel_num_;
	std::map<int,ChannelCache*>                   channel_cache_map_; 
	std::string                                   douban_url_;
	music_logic::GetSongUrl*                      get_song_engine_;
};

class CacheManagerOp{
private:
	static MusicCacheManager   *music_cache_manager_;
	CacheManagerOp () {}
	virtual ~CacheManagerOp() {}
public:
	static MusicCacheManager* GetMusicCache(){
		if (music_cache_manager_==NULL)
			music_cache_manager_ = new MusicCacheManager();
		return music_cache_manager_;
	}

	static void FreeMusicCache(){
		delete music_cache_manager_;
	}
};

}
#endif