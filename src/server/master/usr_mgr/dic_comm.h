#ifndef _MASTER_PLUGIN_USR_MGR_DIC_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DIC_COMM_H__
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <list>
namespace storage{

class RedisComm{
public:
	RedisComm(){}
	virtual ~RedisComm(){}
	static base_storage::DictionaryStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
private:
	static std::list<base::ConnAddr>  addrlist_;
};

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

	static bool SetString(const char* key,const size_t key_len,
		const char* data, size_t len);

	static bool GetString(const char* key,const size_t key_len,
		char** data,size_t* len);

private:
	static base_storage::DictionaryStorageEngine* engine_;
};

}
#endif