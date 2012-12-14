#ifndef MIG_FM_PUBLIC_STORAGE_REDIS_STORAGE_IMPL_H__
#define MIG_FM_PUBLIC_STORAGE_REDIS_STORAGE_IMPL_H__
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>
#include "basic/basic_info.h"
#include "log/mig_log.h"
#include "storage/storage.h"
#include "storage/redis_warrper.h"

namespace base_storage{
	
class RedisStorageEngineImpl:public DictionaryStorageEngine{

public:
    RedisStorageEngineImpl();
    virtual ~RedisStorageEngineImpl();

    virtual bool Connections(std::list<base::ConnAddr>& addrlist);
 
    virtual bool Release();
   
    virtual bool SetValue(const char* key,const size_t key_len,
                          const char* val,const size_t val_len);

    virtual bool AddValue(const char* key,const size_t key_len,
                          const char* val,const size_t val_len);
  
    virtual bool ReplaceValue(const char* key,const size_t key_len,
                              const char* val,const size_t val_len);

    virtual bool GetValue(const char* key,const size_t key_len,
                          char** val,size_t* val_len);

    virtual bool DelValue(const char* key,const size_t key_len);
  
    virtual bool MGetValue(const char* const * key_array,
                           const size_t *key_len_array,size_t element_count);

    virtual bool FetchValue(const char* key,size_t *key_len,
                            char** value,size_t *val_len);
    
    //list
    virtual bool AddListElement(const char* key,const size_t key_len,
                                const char* val,const size_t val_len);
    
    virtual bool GetListElement (const char* key,const size_t key_len,
                                 const int index,char** val,size_t *val_len);
    
    virtual bool DelListElement(const char* key,const size_t key_len,
                                const int index);
    
    virtual bool SetListElement(const int index,const char* key,const size_t key_len,
                                const char* val,const size_t val_len);
    
    virtual bool GetListAll(const char* key,const size_t key_len,std::list<std::string>& list);

private:
	bool PingRedis();
	void Init();
private:
	warrper_redis_context_t*    c_;
}; 

}
/*class RedisStorageEngineImpl: public StorageEngine{

public:
    RedisStorageEngineImpl();
    virtual ~RedisStorageEngineImpl();

    virtual bool Connections(std::list<ConnAddr>& addrlist);
 
    virtual bool Release();
   
    virtual bool SetValue(const char* key,const size_t key_len,const char* val,const size_t val_len,const BaseStorage* base);

    virtual bool AddValue(const char* key,const size_t key_len,const char* val,const size_t val_len,const BaseStorage* base);
  
    virtual bool ReplaceValue(const char* key,const size_t key_len,const char* val,const size_t val_len,const BaseStorage* base);

    virtual bool GetValue(const char* key,const size_t key_len,char** val,size_t* val_len,BaseStorage** base);

    virtual bool DelValue(const char* key,const size_t key_len);
  
    virtual bool MGetValue(const char* const * key_array,const size_t *key_len_array,size_t element_count);

    virtual bool FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len);
/*private:
	bool PingRedis();
private:
	warrper_redis_context_t*    c_;*/

//};
#endif