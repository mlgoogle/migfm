#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
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
	static void Init(std::list<base::ConnAddr>& addrlist);
	
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


private:
	static std::list<base::ConnAddr>  addrlist_;
};

}
#endif
