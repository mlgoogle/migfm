#ifndef _MASTER_PLUGIN_MAMANGER_MGR_DB_COMM__H__
#define _MASTER_PLUGIN_MAMANGER_MGR_DB_COMM__H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "logic_comm.h"
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include <list>
namespace manager_storage{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 10);

	static void Dest();

#if defined (_DB_POOL_)
	static base_storage::DBStorageEngine* DBConnectionPop(void);
	static void DBConnectionPush(base_storage::DBStorageEngine* engine);
#endif
public:
	static bool RecordMessage(const int64 platform_id,const int64 fid,const int64 tid,const int64 msg_id,
								  const std::string& message,const std::string& current_time);

	static bool GetUsersInfo(const int64 from,const int64 count,std::list<int64>& userinfo);
	static bool GetPushMessage(std::string& summary,std::string& message);
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
