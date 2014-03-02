#ifndef _MIG_FM_PUBLIC_STORAGE_DIC_STORAGE__H__
#define _MIG_FM_PUBLIC_STORAGE_DIC_STORAGE__H__

#include <stdio.h>
#include "storage/storage.h"

namespace base_storage{

class MemDic{
public:
	MemDic();
	virtual ~MemDic();

	static bool Init(std::list<base::ConnAddr>& addrlist);

	static bool SetString(const char* key,const size_t key_len,
		const char* data, size_t len);

	static bool GetString(const char* key,const size_t key_len,
		char** data,size_t* len);
private:
	static base_storage::DictionaryStorageEngine*     mem_engine_;
};

}
#endif