#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <list>
#include <vector>
namespace storage{

class DBComm{
public:
	DBComm(){}
	virtual ~DBComm(){}
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 10);
	
	static void Dest();
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