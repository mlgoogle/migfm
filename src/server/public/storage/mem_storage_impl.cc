#include "mem_storage_impl.h"
#include <stdio.h>
#include <stdlib.h>
#include "basic/linuxlist.h"
#include "log/mig_log.h"

#define mcached_is_allocated(__object) ((__object)->options.is_allocated)

namespace base_storage{
	
MemStorageEngineImpl::MemStorageEngineImpl(){

}

MemStorageEngineImpl::~MemStorageEngineImpl(){

}

bool MemStorageEngineImpl::Connections(std::list<base::ConnAddr>& addrlist){
	
    memcached_server_st *servers = NULL;
    memcached_return rc;
    struct list_head *node;
    base::ConnAddr addr;
    cache_ = memcached_create(NULL);
    
    if(cache_==NULL||!mcached_is_allocated(cache_))
        return false;

    memcached_behavior_set(cache_,MEMCACHED_BEHAVIOR_BINARY_PROTOCOL,1);
   
    std::list<base::ConnAddr>::iterator it = addrlist.begin();
    while(it!=addrlist.end()){
    	addr = (*it);
    	servers = memcached_server_list_append(servers,addr.host().c_str(),addr.port(),&rc);
        MIG_INFO(USER_LEVEL,"memcached ip[%s] port[%d]",addr.host().c_str(),addr.port());
    	++it;
    }
    /*list_for_each(node,addrlist) {
        addr = list_entry(node,const connaddr_t,link);
         servers = memcached_server_list_append(servers,addr->ip.c_str(),
                      addr->port,&rc);
    }*/ 

    rc = memcached_server_push(cache_,servers);
    if(memcached_failed(rc)){
        memcached_server_free(servers);
        return false;
    }

    memcached_server_free(servers);
#if defined (MEM_POOL)
    MemPoolInit(8,64); 
#endif
    return true;
}

bool MemStorageEngineImpl::Release(){
    memcached_reset((memcached_st*)cache_);
    memcached_free((memcached_st*)cache_);
	delete this;
}

bool MemStorageEngineImpl::SetValue(const char* key,const size_t key_len,const char* val,
									const size_t val_len){

    memcached_return_t rc;
    mcache_t* cache;
    cache = GetMemCache();
    rc = memcached_set((memcached_st*)cache,key,key_len,val,val_len,0,0);
    PutMemCache(cache);
    if(memcached_success(rc))
        return true;
    return false;
}

bool MemStorageEngineImpl::AddValue(const char* key,const size_t key_len,const char* val,
									const size_t val_len){

    memcached_return_t rc;
    mcache_t* cache;
    cache = GetMemCache();
    rc = memcached_add((memcached_st*)cache,key,key_len,val,val_len,0,0);
    PutMemCache(cache);
    if(memcached_success(rc))
    	return true;
    return false;
}

bool MemStorageEngineImpl::ReplaceValue(const char* key,const size_t key_len,
										const char* val,const size_t val_len){
    memcached_return_t rc;
    mcache_t* cache;
    cache = GetMemCache();
    rc = memcached_replace((memcached_st*)cache,key,key_len,
				val,val_len,0,0);
    PutMemCache(cache);
    if(memcached_success(rc))
	return true;
    return false;
}

bool MemStorageEngineImpl::GetValue(const char* key,const size_t key_len,
				     				char**val,size_t* val_len){
    memcached_return_t rc;
    uint32_t flags = 0;
    char* val_ = NULL;
    mcache_t* cache;
    cache = GetMemCache();
    val_ = memcached_get((memcached_st*) cache_, key, key_len, val_len,
    					 &flags, &rc);
    PutMemCache(cache);

    if(rc==MEMCACHED_END)
	return false;
    else if(memcached_failed(rc))
        return false;
   *val = val_;
    return true;
}

bool MemStorageEngineImpl::DelValue(const char* key,const size_t key_len){

    memcached_return_t rc;
    mcache_t* cache;
    cache = GetMemCache();
    rc = memcached_delete ((memcached_st*)cache_,key,key_len,0);
    PutMemCache(cache);
    if(memcached_success(rc))
        return true;
    return false;
}

bool MemStorageEngineImpl::MGetValue(const char* const * key_array,
		const size_t* key_len_array,size_t element_count){

    memcached_return_t rc;
    rc = memcached_mget((memcached_st*)cache_,key_array,key_len_array,
			element_count);
    if(rc==MEMCACHED_END)
	  return false;
    else if(memcached_failed(rc))
	  return false;
    return true;
}

bool MemStorageEngineImpl::FetchValue(char* key,size_t *key_len,
									  char** value,size_t *val_len){
   if (NULL == value)
	   return false;

    uint32_t flags = 0;
    memcached_return_t rc;
    *value = memcached_fetch((memcached_st*)cache_, key,
    						key_len,val_len,&flags, &rc);
	if (rc == MEMCACHED_END)
		return false;
	else if (memcached_failed(rc))
		return false;
	return true;
}

bool MemStorageEngineImpl::IncDecValue(const char* key, size_t key_len,
		int64 incby, int64& result) {
	return true;
}

bool MemStorageEngineImpl::IncrValue(const char *key, const size_t key_len, 
									 const char *val, const size_t val_len){
    return true;
} 

bool MemStorageEngineImpl::DecrValue(const char *key, const size_t key_len, 
									 const char *val, const size_t val_len){
    return true;
}
/*
int main(int argc, char *argv[]) 
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers;
    char value[8191];

    //connect multi server
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    servers = memcached_server_list_append(servers, "localhost", 11212, &rc);
    rc = memcached_server_push(memc, servers);
    memcached_server_free(servers);

    //Save multi data
    size_t i;
    char *keys[]= {"key1", "key2", "key3"};
    size_t key_length[]= {4, 4, 4};
    char *values[] = {"This is c first value", "This is c second value", "This is c third value"};
    size_t val_length[]= {21, 22, 21};
    for (i=0; i <3; i++) {
    rc = memcached_set(memc, keys[i], key_length[i], values[i], val_length[i], (time_t)180,(uint32_t)0);
    if (rc == MEMCACHED_SUCCESS) {
        printf("Save key:%s data:/"%s/" success./n", keys[i], values[i]);
    }
    }

    //Fetch multi data
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;
    uint32_t flags;
    rc = memcached_mget(memc, keys, key_length, 3);
    while ((return_value = memcached_fetch(memc, return_key, &return_key_length, &return_value_length, &flags, &rc))) {
    if (rc == MEMCACHED_SUCCESS) {
        printf("Fetch key:%s data:%s/n", return_key, return_value);
    }
    }
    
    //Delete multi data
    for (i=0; i <3; i++) {
    rc = memcached_set(memc, keys[i], key_length[i], values[i], val_length[i], (time_t)180, (uint32_t)0);
    rc = memcached_delete(memc, keys[i], key_length[i], (time_t)0);
    if (rc == MEMCACHED_SUCCESS) {
        printf("Delete %s success/n", keys[i], values[i]);
    }
    }
    //free
    memcached_free(memc);
    return 0;
} 
?????????
[root@localhost html]# gcc -o cmultmem cmultmem.c -lmemcached 
[root@localhost html]# ./cmultmem //???
Save key:key1 data:"This is c first value" success.
Save key:key2 data:"This is c second value" success.
Save key:key3 data:"This is c third value" success.
Fetch key:key2 data:This is c second value
Fetch key:key3 data:This is c third value
Fetch key:key1 data:This is c first value
Delete key1 success
Delete key2 success
Delete key3 success 
*/


bool MemStorageEngineImpl::AddListElement(const char* key,
										  const size_t key_len,
										  const char* val,
										  const size_t val_len,
										  const int flag){
	return true;
}

bool MemStorageEngineImpl::GetListElement (const char* key,const size_t key_len,
										   const int index,char** val,size_t *val_len){
	return true;								
}

bool MemStorageEngineImpl::DelListElement(const char* key,const size_t key_len,const int index){
	return true;
}

bool MemStorageEngineImpl::SetListElement(const int index,const char* key,const size_t key_len,
										  const char* val,const size_t val_len){
	return true;
}

bool MemStorageEngineImpl::PopListElement(const char* key,const size_t key_len,char** val,
        		size_t *val_len,const int flag){
	return true;
}

bool MemStorageEngineImpl::GetListAll(const char* key,const size_t key_len,
	                                  std::list<std::string>& list){
	return true;
}

bool MemStorageEngineImpl::GetHashValues(const char *hash_name, 
										 const size_t hash_name_len,
										 std::list<std::string> &list){

    return true;

}

bool MemStorageEngineImpl::GetAllHash(const char* hash_name, 
									  const size_t hash_name_len, 
									  std::map<std::string,std::string>& map){
    return true;
}

#if defined (MEM_POOL)
bool MemStorageEngineImpl::MemPoolInit(int32 init,int32 max){ 
   cached_pool_ = (mcached_pool_t*)memcached_pool_create((memcached_st*)cache_,ini,max);
    return true;
}


mcache_t* MemStorageEngineImpl::MemGetPool(void){
    memcached_return_t rc;
    return (mcached_t*)memcached_pool_pop((memcached_pool_st*)cached_pool_,false,&rc);
}

int MemStorageEngineImpl::MemPutPool(mcached_t* conn){
    return memcached_pool_push((memcached_pool_st*)pool,conn);
}

#endif

mcache_t* MemStorageEngineImpl::GetMemCache(){

#if defined (MEM_POOL)
    return MemGetPool();
#else
    return cache_;
#endif
}

void MemStorageEngineImpl::PutMemCache(mcache_t* conn){
#if defined(MEM_POOL)
    MemPutPool(conn);
#endif
}

}
