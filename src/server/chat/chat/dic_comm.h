#ifndef _CHAT_DIC_COMM_H__
#define _CHAT_DIC_COMM_H__
#include "chat_basic_infos.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <libmemcached/memcached.h>

namespace chat_storage{

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static bool Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
	static bool CheckToken(const int64 platform_id,int64 user_id,
		const char* current_token);
	static bool GetUserInfo(const int64 platform_id,int64 user_id,
		                    chat_base::UserInfo& userinfo);
private:
	static base_storage::DictionaryStorageEngine* engine_;
};

class RedisComm{
public:
	RedisComm(){}
	virtual ~RedisComm(){}
	static base_storage::DictionaryStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

	static bool SetUserPushConfig(int64 uid, const std::string &device_token,
			int is_receive, unsigned begin_time, unsigned end_time);

	static bool GetUserPushConfig(int64 uid, std::string &device_token,
			bool &is_receive, unsigned &begin_time, unsigned &end_time);
private:
	static std::list<base::ConnAddr>   addrlist_;
};
}

#endif
