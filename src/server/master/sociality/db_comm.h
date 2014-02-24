#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <list>
#include <vector>

namespace mig_sociality {

class DBComm{
public:
	DBComm(){}
	virtual ~DBComm(){}
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 10);
	
	static void Dest();

public:

	static bool GetMusicUser(const std::string& uid,
							 const std::string& fromid,
							 const std::string& count,
							 std::vector<std::string>& vec_users,
							 std::list<struct MusicFriendInfo>& userlist);

	static bool GetUserInfos(const std::string& uid, std::string& nickname,
			std::string& gender, std::string &head);

	static bool GetUserInfos(const std::string& uid, std::string& nickname,
		std::string& gender, std::string &head,double& latitude,
		double& longitude);

	static bool GetWXMusicUrl(const std::string& song_id,std::string& song_url,
			std::string& dec,std::string& dec_id,std::string& dec_word);

	static bool AddFriend(const std::string &uid, const std::string &touid);

	static bool AddMusciFriend(const std::string& uid,
							   const std::string &touid);

	static bool GetMusicUrl(const std::string& song_id,std::string& hq_url,
		std::string& song_url);

	static bool SetMusicHostCltCmt(const std::string& songid,
		const int32 flag, 
		const int32 value=1);

	static bool GetMusicOtherInfos(std::map<std::string,base::MusicInfo> &                                         song_music_infos);

	struct FriendInfo {
		std::string uid;
		std::string name;
		uint32		type;
	};
	typedef std::vector<FriendInfo> FriendInfoList;
	static bool GetFriendList(const std::string &uid, FriendInfoList &friends);

	static bool GetUserInfos(int64 uid,
			std::string& nickname, std::string& gender,
			std::string& type, std::string& birthday,
			std::string& location, std::string& source,
			std::string& head);

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
#endif // mig_sociality
