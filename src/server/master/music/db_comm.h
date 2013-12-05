#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
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

	static bool GetUserHistoryMusic(const std::string& uid,const std::string& fromid,
		const std::string& count,std::list<std::string>& songlist);

private:
#if defined (_DB_POOL_)	
	static base_storage::DBStorageEngine* CreateConnection(void);
	static base_storage::DBStorageEngine* DBConnectionPop(void);
	static void DBConnectionPush(base_storage::DBStorageEngine* db);
#endif
private:
	static std::list<base::ConnAddr>  addrlist_;
#if defined (_STORAGE_POOL_)
	static base_storage::DBStorageEngine**       db_conn_pool_;
	static int32                                 db_conn_num_;
	static threadrw_t*                           db_pool_lock_;
#endif
};

}
#endif