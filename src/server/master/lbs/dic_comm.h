#ifndef _MASTER_PLUGIN_USR_MGR_DIC_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DIC_COMM_H__
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <list>
#include <vector>
#include <string>
#include <map>

namespace storage {

class RedisComm{
public:
	RedisComm(){}
	virtual ~RedisComm(){}
	static base_storage::DictionaryStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
	static bool GetMusicInfos(const std::string& key,std::string& music_infos);
	static bool IsCollectSong(const std::string& uid,const std::string& songid);
	static bool GetMusicAboutUser(const std::string &songid,std::string& content);
	static bool GetCollectSongs(const std::string& uid,std::map<std::string,std::string>& collect);
	static bool RecordCommect(const std::string& uid);

	static bool GetMsgCount(const std::string& uid, int& count);
private:
	static std::list<base::ConnAddr>  addrlist_;
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

public:
	// 获取用户当前唱的歌
	static bool GetUserCurrentSong(const std::vector<std::string> &vec_user,
		std::map<std::string, std::string> &map_songs);

private:
	static base_storage::DictionaryStorageEngine* engine_;
};

}
#endif
