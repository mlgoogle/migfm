#ifndef _MASTER_PLUGIN_MUSIC_MGR_DIC_COMM_H__
#define _MASTER_PLUGIN_MUSIC_MGR_DIC_COMM_H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include "basic/radom_in.h"
#include <libmemcached/memcached.h>
#include <list>
#include <vector>
#include <hiredis.h>
#include "json/json.h"

struct redisReply;

namespace mig_sociality {

class RedisComm{
public:
	RedisComm(){}
	virtual ~RedisComm(){}
	static base_storage::DictionaryStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,
			const int32 dic_conn_num = 10);
	static void Dest();

	static bool SetUserPushConfig(int64 uid, const std::string &device_token,
			int is_receive, unsigned begin_time, unsigned end_time);

	static bool GetUserPushConfig(int64 uid, std::string &device_token,
			bool &is_receive, unsigned &begin_time, unsigned &end_time);

	static bool GetMusicInfos(const std::string& songid, std::string &music_infos);

	static bool GenaratePushMsgID(int64 uid, int64 &msg_id);

	static bool StagePushMsg(int64 uid, int64 msg_id, const std::string &msg);

	static bool GetStagedPushMsg(int64 uid, int page_index, int page_size, std::list<std::string> &msgs);

	static bool AddFriend(int64 uid, int64 touid);

	static bool GetFriensList(int64 uid, std::list<std::string> &friends);

	static bool SaveSongComment(int64 songid, int64 uid, const std::string &comment,
		                        const std::string& curjson);

	static bool ReadSongComment(int64 songid, int64 from_id, int count, int64 &total, Json::Value &result);

	static void SetMusicAboutUser(const std::string& songid,
		const std::string& hot_num,const std::string& cmt_num,
		const std::string& clt_num);

	static bool GetMusicAboutUser(const std::string &songid,std::string& content);

	static bool RecordingMsg(const std::string& uid,const base::NormalMsgInfo& msg);

	static bool IsCollectSong(const std::string& uid,const std::string& songid);

	static bool GetCollectSongs(const std::string& uid,
		std::map<std::string,std::string>& song_map);

	static bool AddNewMessage(const int64 uid,int64& new_msg);


	static bool ClearNewMessage(const int64 uid);

	static void GetPushMessageMusicinfos(std::list<struct MessageListInfo>& list);


	static bool GetMusicInfos(std::map<std::string,std::string>&temp_songinfo,
		               std::map<std::string,base::MusicInfo>& songinfo);

	static base_storage::CommandReply* _CreateReply(redisReply* reply);
public:
#if defined (_DIC_POOL_)
	static base_storage::DictionaryStorageEngine* RedisConnectionPop(void);
	static void RedisConnectionPush(base_storage::DictionaryStorageEngine* engine);
#endif

private:
	static std::list<base::ConnAddr>   addrlist_;
	static base::MigRadomIn*              radom_num_;
#if defined (_DIC_POOL_)
	static std::list<base_storage::DictionaryStorageEngine*>  dic_conn_pool_;
	static threadrw_t*                                        dic_pool_lock_;
#endif
};

class AutoDicCommEngine{
public:
	AutoDicCommEngine();
	virtual ~AutoDicCommEngine();
	base_storage::DictionaryStorageEngine*  GetDicEngine(){
		//if(engine_){engine_->Release();}
		return engine_;
	}
private:
	base_storage::DictionaryStorageEngine*  engine_;
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

	static bool GetCurrentSong(const std::string& key,std::string& value);

    static bool GetUserCurrentSong(const std::vector<std::string> &vec_user,
		std::map<std::string, std::string> &map_songs);
private:
	static base_storage::DictionaryStorageEngine* engine_;
};

}
#endif // mig_sociality
