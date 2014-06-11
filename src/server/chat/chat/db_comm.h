#ifndef __CHAT_DB_COMM_H__
#define __CHAT_DB_COMM_H__

#include "chat_basic_infos.h"
#include "base/logic_comm.h"
#include "basic/basictypes.h"
#include "storage/storage.h"
#include "basic/basic_info.h"

namespace chat_storage{

class DBComm{
public:
	DBComm(){};
	virtual ~DBComm(){};
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 5);

	static void Dest();

	static bool GetUserInfo(const int64 platform_id,int64 user_id,
		                     chat_base::UserInfo& userinfo);

	static bool GetRobotsUserInfo(const int64 platform_id,int64 user_id,
							 chat_base::UserInfo& userinfo);

	static bool RecordMessage(const int64 platform_id,const int64 fid,const int64 tid,const int64 msg_id,
							  const std::string& message,const std::string& current_time);

	static bool GetUserLBSPos(const int64& uid,double& latitude,double& longitude);

	static bool RecordUserMessageList(const int32 type,const int64 send_uid,
									 const int64 to_uid,
									 const double distance,
									 const std::string& message);

public:
#if defined (_DB_POOL_)
	static base_storage::DBStorageEngine* DBConnectionPop(void);
	static void DBConnectionPush(base_storage::DBStorageEngine* engine);
#endif
public:
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
#endif
