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

struct redisReply;

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

    virtual bool FetchValue(char* key,size_t *key_len,
                            char** value,size_t *val_len);

	virtual bool IncrValue(const char* key,const size_t key_len,
		const char* val,const size_t val_len);

	virtual bool DecrValue(const char* key,const size_t key_len,
		const char* val,const size_t val_len);
    
    virtual bool IncDecValue(const char* key, size_t key_len, int64 incby, int64 &result);

    //list
    virtual bool AddListElement(const char* key,const size_t key_len,
		                        const char* val,const size_t val_len,
		                        const int flag);
    
    virtual bool GetListElement (const char* key,const size_t key_len,
                                 const int index,char** val,size_t *val_len);
    
    virtual bool DelListElement(const char* key,const size_t key_len,
                                const int index);
    
    virtual bool PopListElement(const char* key,const size_t key_len,char** val,
        		size_t *val_len,const int flag);

    virtual bool SetListElement(const int index,const char* key,const size_t key_len,
                                const char* val,const size_t val_len);
    

    virtual bool GetListAll(const char* key,const size_t key_len,std::list<std::string>& list);

	virtual int GetListSize(const char* list_name);

	virtual bool AddHashRadomElement(const char* hash_name,const char* val,const size_t val_len);

	virtual bool GetHashRadomElement(const char* hash_name,char** val,size_t *val_len,
		                             const int radom_num);

	virtual bool DelHashRadomElement(const char* hash_name);

	virtual bool AddHashElement(const char* hash_name,const char* key,const size_t key_len,
		const char* val,const size_t val_len);

	virtual bool SetHashElement(const char* hash_name,const char* key,const size_t key_len,
		const char* val,const size_t val_len);

	virtual bool GetHashElement(const char* hash_name,const char* key,const size_t key_len,
		char** val,size_t *val_len);

	virtual bool DelHashElement(const char* hash_name,const char* key,const size_t key_len);

	virtual int  GetHashSize(const char* hash_name);

	virtual bool GetHashValues(const char* hash_name,
		                       const size_t hash_name_len,
		                       std::list<std::string>& list);

	virtual bool GetAllHash(const char* hash_name,
		                    const size_t hash_name_len,
		                    std::map<std::string,std::string>& map);

	virtual bool GetListRange(const char* key,const size_t key_len,
				int from, int to, std::list<std::string>& list,
				const int flag);

	virtual CommandReply *DoCommand(const char *format/*, ...*/);

	virtual void *GetContext() {
		if(PingRedis()!=1) return NULL;
		else return c_->context; }
private:
	bool PingRedis();
	void Init();

	CommandReply *_CreateReply(redisReply *reply);
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
