#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include "json/json.h"
#include <list>

namespace storage{

class DBComm {
public:
	DBComm(){}
	virtual ~DBComm(){}
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,
			  const int32 db_conn_num = 10);
	
	static void Dest();

	static bool GetUserInfos(const std::string& uid,
							std::string& nickname,std::string& gender,
							std::string& pic);

	static bool GetMusicUrl(const std::string& song_id,std::string& hq_url,
		std::string& song_url);

	static bool GetMusicAboutInfo(const std::string& song_id,std::string& hq_url,
			std::string& song_url,std::string& clt_num,std::string& cmt_num,
			std::string& hot_num);

	static bool UpDateUserLbsPos(Json::Value& users,const int64 src_uid);

	static bool GetSameMusic(Json::Value& users,const int64 src_uid,const double latitude,
	                        const double longitude);

	static bool GetMusicFriendNum(const std::string& uid,
		                          std::string& snum);

	static bool GetUserLbsPos(const int64 src_uid,double& latitude,
		                      double& longitude);

	static bool GetLBSAboutInfos(const std::string& uid,std::string& sex,std::string& nickname,
							std::string& head,std::string& birthday,double& latitude,
							double& longitude);



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

#if defined (_DB_SINGLE_)
	static base_storage::DBStorageEngine*             db_conn_single_;
	static threadrw_t*                                db_single_lock_;
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
