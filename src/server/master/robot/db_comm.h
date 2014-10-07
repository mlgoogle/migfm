#ifndef _MASTER_PLUGIN_CHAT_MGR_DB_COMM__H__
#define _MASTER_PLUGIN_CHAT_MGR_DB_COMM__H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "logic_comm.h"
#include "robot_basic_infos.h"
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include <list>
#include <string>
namespace robot_storage{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 5);

	static void Dest();

public:
	static bool GetNewMusicInfo(std::list<robot_base::NewMusicInfo>& music_list);

	static bool GetVailedLyric(std::list<int64>& music_list,const int64 from,
			const int64 count);

	static bool GetMailUserInfo(const int64 count,const int64 from,
					std::list<robot_base::MailUserInfo>& user_list);

	static bool GetRobotsInfo(const int64 count, const int64 from,
			std::list<robot_base::RobotInfo>& user_list);

	static bool GetSpreadMail(std::string& title,std::string& content);

	static bool UpdateMusicUrl(const int64 id,const std::string& url);

	static bool UpdateLyric(const int64 id,const std::string& lyric);

	static bool AddMusicInfo(const int64 id,const std::string& title,const std::string& album,const std::string& artist,
			const std::string& pub_time,const std::string& album_pic,
			const std::string& mp3_url,std::string& songid);

	static bool UpdateHeadUrl(const int64 id,const std::string& mp3_url);

	static bool GetVailedUrlMusic(std::list<int64>& music_list);

	static bool GetSinaWbToken(std::list<robot_base::SINAWBAccessToken*>& list);
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
	base_storage::DBStorageEngine*  GetDBEngine(){
		if(engine_){engine_->Release();}
		return engine_;
	}
private:
	base_storage::DBStorageEngine*  engine_;
};

}

#endif
