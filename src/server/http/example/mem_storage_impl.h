#ifndef MIG_FM_MEM_STORAGE_IMPLE_H__
#define MIG_FM_MEM_STIRAGE_IMPLE_H__
#include <list>
#include <string>
#include <libmemcached/memcached.h>
#include <libmemcachedutil-1.0/pool.h>
#include "conn_addr.h"
#include "storage.h"
typedef memcached_st mcache_t;
class MemStorageEngineImpl: public StorageEngine{

public:
    MemStorageEngineImpl();
    virtual ~MemStorageEngineImpl();

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
    mcache_t*             cache_; 

}; 
#endif
