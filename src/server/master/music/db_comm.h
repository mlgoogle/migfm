#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <list>
#include <vector>
#include <string>
#include <map>
namespace storage{

class DBComm{
public:
	DBComm(){}
	virtual ~DBComm(){}
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 10);
	
	static void Dest();

	static bool GetChannelInfo(std::vector<base::ChannelInfo>& channel,int& num);
	static bool GetDescriptionWord(std::list<base::WordAttrInfo>& word_list,int flag);
	static bool GetMusicUrl(const std::string& song_id,std::string& hq_url,
		                    std::string& song_url);
	static bool GetWXMusicUrl(const std::string& song_id,std::string& song_url,
		                      std::string& dec,std::string& dec_id,
							  std::string& dec_word);
	static bool GetMoodParentWord(std::list<base::WordAttrInfo>& word_list);

	static bool SetMusicHostCltCmt(const std::string& songid,const int32 flag,
		               const int32 value);

	static bool GetSongidFromDoubanId(const std::string& douban_songid,
		                              std::string& songid);

	static bool GetMusicOtherInfos(std::map<std::string,base::MusicInfo> &song_music_infos);

	static bool RecordMusicHistory(const std::string& uid,const std::string& songid);

	static bool GetUserHistoryMusic(const std::string& uid,
		const std::string& fromid,const std::string& count,
		std::list<std::string>& songlist);

	static bool GetChannelInfos(std::list<int>& list);

	static bool GetMoodInfos(std::list<int>& list);

	static bool GetSceneInfos(std::list<int>& list);

public:
#if defined (_DB_POOL_)	
	static base_storage::DBStorageEngine* DBConnectionPop(void);
	static void DBConnectionPush(base_storage::DBStorageEngine* engine);
#endif
private:
	static std::list<base::ConnAddr>  addrlist_;
#if defined (_DB_POOL_)
	static std::list<base_storage::DBStorageEngine*>  db_conn_pool_;
	static threadrw_t*                                db_pool_lock_;
#endif
};

class AutoDBCommEngine{
public:
	AutoDBCommEngine();
	virtual ~AutoDBCommEngine();
	base_storage::DBStorageEngine*  GetDBEngine(){return engine_;}
private:
	base_storage::DBStorageEngine*  engine_;
};


}
#endif
