#ifndef _MIG_FM_PUBLIC_STORAGE_DIC_SERIALIZATION__H__
#define _MIG_FM_PUBLIC_STORAGE_DIC_SERIALIZATION__H__

#include <stdio.h>
#include "storage/storage.h"

namespace base_storage{

class MemDicSerial{
public:
    MemDicSerial();
    virtual ~MemDicSerial();
    static bool Init(std::list<base::ConnAddr>& addrlist);
    static bool SetString(const char* key,const size_t key_len,
                          const char* data, size_t len);
    static bool GetString(const char* key,const size_t key_len,
                          char** data,size_t* len);
private:
    static base_storage::DictionaryStorageEngine*     mem_engine_;
};


class RedisDicSerial{
public:
	RedisDicSerial();
	virtual ~RedisDicSerial();
	static bool Init(std::list<base::ConnAddr>& addrlist);
private:
	static base_storage::DictionaryStorageEngine*    redis_engine_;
	
};

}
#endif
