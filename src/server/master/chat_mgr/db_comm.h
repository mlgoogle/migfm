#ifndef _MASTER_PLUGIN_CHAT_MGR_DB_COMM__H__
#define _MASTER_PLUGIN_CHAT_MGR_DB_COMM__H__

#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include "logic_comm.h"
#include <list>
namespace chat_storage{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,const int32 db_conn_num = 10);

	static void Dest();

public:
	static bool GetLeaveMessage(const int64 platform_id,const int64 uid,const int64 oppid,
			 const int32 from,const int32 count,std::list<struct GetLeaveMessage*>& list);
private:
	static std::list<base::ConnAddr>  addrlist_;
};
}

#endif
