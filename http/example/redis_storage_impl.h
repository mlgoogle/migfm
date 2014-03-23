#ifndef MIG_FM_REDIS_STORAGE_IMPL_H__
#define MIG_FM_REDIS_STORAGE_IMPL_H__
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>
#include "conn_addr.h"
#include "storage.h"
#include "redis_warrper.h"
class RedisStorageEngineImpl: public StorageEngine{

public:
    RedisStorageEngineImpl();
    virtual ~RedisStorageEngineImpl();

    virtual bool Connections(std::list<ConnAddr>& addrlist);
 
    virtual bool Release();
   
    virtual bool SetValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL);

    virtual bool AddValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL);
  
    virtual bool ReplaceValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL);

    virtual bool GetValue(const char* key,const size_t key_len,char** val,size_t* val_len,struct BaseStorage** base=NULL);

    virtual bool DelValue(const char* key,const size_t key_len);
  
    virtual bool MGetValue(const char* const * key_array,const size_t *key_len_array,size_t element_count);

    virtual bool FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len);
    
    //list
    virtual bool AddListElement(const char* key,const size_t key_len,const char* val,const size_t val_len);
    
    virtual bool GetListElement (const char* key,const size_t key_len,const int index,char** val,size_t *val_len);
    
    virtual bool DelListElement(const char* key,const size_t key_len,const int index);
    
    virtual bool SetListElement(const int index,const char* key,const size_t key_len,const char* val,const size_t val_len);
    
    virtual bool GetListAll(const char* key,const size_t key_len,std::list<std::string>& list);
private:
	bool PingRedis();
private:
	warrper_redis_context_t*    c_;
}; 

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