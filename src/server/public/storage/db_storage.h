#ifndef _MIG_FM_PUBLIC_STORAGE_DB_STORAGE__H__
#define _MIG_FM_PUBLIC_STORAGE_DB_STORAGE__H__

#include "storage/storage.h"
#include "basic/basic_info.h"
#include <stdio.h>

namespace base_storage{

class MYSQLDB{
public:
	MYSQLDB();
	virtual ~MYSQLDB();

	static bool Init(std::list<base::ConnAddr>& addrlist);

	static bool GetUserInfo(const std::string& uid,base::UserInfo& usrinfo);

	static bool CheckConnection();

private:
	static base_storage::DBStorageEngine*     mysql_engine_;
	static std::list<base::ConnAddr>          addrlist_;
};

}
#endif
