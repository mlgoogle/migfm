#ifndef _MASTER_PLUGIN_MUSIC_MGR_DIC_COMM_H__
#define _MASTER_PLUGIN_MUSIC_MGR_DIC_COMM_H__
#include "storage/storage.h"
#include "basic/basic_info.h"
#include "basic/radom_in.h"
#include "storage/storage.h"
#include <hiredis.h>
#include <list>
#include <vector>

namespace storage{

class RedisComm{
public:
	RedisComm(){}
	virtual ~RedisComm(){}
	static base_storage::DictionaryStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

	static bool GetMoodAndScensId(const std::string& key,std::string& word_id);

	static bool GetMusicMapRadom(const std::string& art_name,std::string& song_id);

	static bool GetMusicInfos(const std::string& key,std::string& music_infos);

	static bool GetUserMoodMap(const std::string& uid,std::string& mood_map);

	static bool SetCollectSong(const std::string& uid,const std::string& songid,
		const std::string& content);

	static bool IsCollectSong(const std::string& uid,const std::string& songid);

	static bool GetCollectSong(const std::string& uid,const std::string& songid,
		                       std::string& content);

	static bool GetCollectSongs(const std::string& uid,std::list<std::string>& song_list);

	static bool DelCollectSong(const std::string& uid,const std::string& songid);

	static bool SetHateSong(const std::string& uid,const std::string& songid);

	static bool IsHateSong(const std::string& uid,const std::string& songid);

	static bool DelHateSong(const std::string& uid,const std::string& songid);

	static bool GetDefaultSongs(const std::string& uid,std::list<std::string>& song_list);

	static bool MgrListenSongsNum(const std::string& songid,
		const std::string& last_songid,const std::string& uid);

	static bool GetUpdateConfig(const std::string& key,std::string& content);

	static int  GetHashSize(const std::string& key);

	static void SetMusicAboutUser(const std::string& songid,const std::string& hot_num,
		                          const std::string& cmt_num,const std::string& clt_num);

	static bool GetMusicAboutUser(const std::string &songid,std::string& content);

	static bool GetMusicHistroyCollect(const std::string &uid,
					const std::string& is_like,std::list<std::string>& songlist,
					std::list<std::string>& collect_list,
					std::list<std::string>& history_list);

	static base_storage::CommandReply* _CreateReply(redisReply* reply);

private:
	static void SetRadomNum();
	static void GetMusicInfos(base_storage::DictionaryStorageEngine*engine,
		std::list<std::string>& songlist,
		std::list<std::string>& songinfolist);
private:
	static std::list<base::ConnAddr>   addrlist_;
	static base::MigRadomIn*              radom_num_;
};

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

	static bool SetString(const char* key,const size_t key_len,
		                  const char* data, size_t len);

	static bool GetString(const char* key,const size_t key_len,
		                  char** data,size_t* len);

	static bool SetUsrCurrentSong(const std::string& uid,
		                          const std::string& songid,
								  const std::string& name,
								  const std::string& singer,
								  const std::string& netstat,
								  const std::string& mode,
								  const std::string& tid);
private:
	static base_storage::DictionaryStorageEngine* engine_;
};

}
#endif