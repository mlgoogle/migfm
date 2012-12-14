#ifndef _MIG_FM_PUBLIC_STORAGE_DIC_SERIALIZATION__H__
#define _MIG_FM_PUBLIC_STORAGE_DIC_SERIALIZATION__H__

#include <stdio.h>
#include "storage/storage.h"

namespace base_storage{

class MemDicSerial{
public:
    MemDicSerial();
    virtual ~MemDicSerial();
    static void Init(std::list<base::ConnAddr>& addrlist);
private:
	static base_storage::DictionaryStorageEngine*     mem_engine_;
};


class RedisDicSerial{
public:
	RedisDicSerial();
	virtual ~RedisDicSerial();
	static void Init(std::list<base::ConnAddr>& addrlist);
private:
	static base_storage::DictionaryStorageEngine*    redis_engine_;
	
};

}
#endif
