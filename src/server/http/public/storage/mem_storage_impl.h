#ifndef MIG_FM_PUBLIC_STORAGE_MEM_STORAGE_IMPLE_H__
#define MIG_FM_PUBLIC_STORAGE_MEM_STIRAGE_IMPLE_H__
#include <list>
#include <string>
#include <libmemcached/memcached.h>
#include <libmemcachedutil-1.0/pool.h>
#include "basic/basic_info.h"
#include "storage/storage.h"
typedef memcached_st mcache_t;

namespace base_storage{
	
class MemStorageEngineImpl: public DictionaryStorageEngine{

public:
    MemStorageEngineImpl();
    virtual ~MemStorageEngineImpl();

    virtual bool Connections(std::list<base::ConnAddr>& addrlist);
 
    virtual bool Release();
   
    virtual bool SetValue(const char* key,const size_t key_len,
    						const char* val,const size_t val_len);

    virtual bool AddValue(const char* key,const size_t key_len,const char* val,const size_t val_len);
  
    virtual bool ReplaceValue(const char* key,const size_t key_len,const char* val,const size_t val_len);

    virtual bool GetValue(const char* key,const size_t key_len,char** val,size_t* val_len);

    virtual bool DelValue(const char* key,const size_t key_len);
  
    virtual bool MGetValue(const char* const * key_array,const size_t *key_len_array,size_t element_count);

    virtual bool FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len);
    
    
        //list
        
    virtual bool AddListElement(const char* key,const size_t key_len,const char* val,const size_t val_len);
    
    virtual bool GetListElement (const char* key,const size_t key_len,const int index,char** val,size_t *val_len);
    
    virtual bool DelListElement(const char* key,const size_t key_len,const int index);
    
    virtual bool SetListElement(const int index,const char* key,const size_t key_len,const char* val,const size_t val_len);

	virtual bool AddHashRadomElement(const char* hash_name,const char* val,const size_t val_len){}

	virtual bool GetHashRadomElement(const char* hash_name,char** val,size_t *val_len){}

	virtual bool DelHashRadomElement(const char* hash_name){}

	virtual bool AddHashElement(const char* hash_name,const char* key,const size_t key_len,
		const char* val,const size_t val_len){}

	virtual bool GetHashElement(const char* hash_name,const char* key,const size_t key_len,
		char** val,size_t *val_len){}

	virtual bool DelHashElement(const char* hash_name,const char* key,const size_t key_len){}
    
    virtual bool GetListAll(const char* key,const size_t key_len,std::list<std::string>& list);

#if defined(MEM_POOL)
private:
    void MemPoolInit(int32 ini,int32 max);
    mcache_t* MemGetPool(void);
    int MemPutPool(mcache_t* conn);
#endif
private:
    mcache_t* GetMemCache();
    void PutMemCache(mcache_t* conn);
private:
    mcache_t*             cache_; 
#if defined (MEM_POOL)
    mcache_pool_t*        cached_pool_;
#endif
}; 

}
#endif
