#ifndef __CHAT_DB_COMM_H__
#define __CHAT_DB_COMM_H__

#include "chat_basic_infos.h"
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
	static void Init(std::list<base::ConnAddr>& addrlist);

	static void Dest();

	static bool GetUserInfo(const int64 platform_id,int64 user_id,
		                     chat_base::UserInfo& userinfo);

	static bool RecordMessage(const int64 platform_id,const int64 fid,const int64 tid,const int64 msg_id,
							  const std::string& message,const std::string& current_time);

private:
	static std::list<base::ConnAddr>  addrlist_;
};

}
#endif
