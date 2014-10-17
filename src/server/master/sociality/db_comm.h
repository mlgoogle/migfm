#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "logic_comm.h"
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
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 5);
	
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

	static bool GetMusicOtherInfos(std::map<std::string,base::MusicInfo>& song_music_infos);

	static bool GetLBSAboutInfos(const std::string& uid,std::string& sex,std::string& nickname,
								std::string& head,std::string& birthday,double& latitude,
								double& longitude);

	static bool RecordUserMessageList(const int32 type,
									 const int64 send_uid,
									 const int64 to_uid,
									 const double distance,
									 const std::string& message);

	static bool RecordUserMessageList(const int32 type,const int64 send_uid,
							const int64 to_uid,std::list<struct RecordMessage>& list);

	static bool GetLyric(const int64 songid,std::string& lyric);

	static bool GetShareInfo(const int64 songid,const std::string& mode,const std::string& index,
				std::string& lyric,std::string& description);

	static bool RecordShareInfo(const int64 uid,const int64 songid,const int64 plat);

	struct FriendInfo {
		std::string uid;
		std::string name;
		uint32		type;
	};
	typedef std::vector<FriendInfo> FriendInfoList;
	static bool GetFriendList(const std::string &uid, FriendInfoList &friends);


	static bool GetMessageList(const int64 uid,const int64 count,const int64 from,
			std::list<struct MessageListInfo>& message_list);

	static bool GetUserInfos(int64 uid,
			std::string& nickname, std::string& gender,
			std::string& type, std::string& birthday,
			std::string& location, std::string& source,
			std::string& head);

public:
#if defined (_DB_POOL_)	
	static base_storage::DBStorageEngine* DBConnectionPop(void);
	static void DBConnectionPush(base_storage::DBStorageEngine* engine);
#endif
public:
	static std::list<base::ConnAddr>  addrlist_;
private:
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
		if(engine_){
			engine_->Release();//释放多余记录集
			if(!engine_->CheckConnect()){//失去连接重新连接
				//重新创建连接
				LOG_DEBUG("lost connection");
				if(!engine_->Connections(DBComm::addrlist_))
					return NULL;
			}
			return engine_;
		}
		return engine_;
	}
private:
	base_storage::DBStorageEngine*  engine_;
};

}
#endif // mig_sociality
