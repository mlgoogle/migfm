#include "redis_storage_impl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "log/mig_log.h"
namespace base_storage{
	
RedisStorageEngineImpl::RedisStorageEngineImpl(){
	Init();
}

RedisStorageEngineImpl::~RedisStorageEngineImpl(){

}

void RedisStorageEngineImpl::Init(){
	c_ = new warrper_redis_context_t;
}

bool RedisStorageEngineImpl::Connections(std::list<base::ConnAddr>& addrlist){
	base::ConnAddr  addr;
	char* str;
	std::list<base::ConnAddr>::iterator it = addrlist.begin();
	while(it!=addrlist.end()){
		addr = (*it);
		str = RedisConnections(&c_,addr.host().c_str(),addr.port());
                MIG_INFO(USER_LEVEL,"redis ip[%s] port[%d]",addr.host().c_str(),
                         addr.port());
		if(str!=NULL)
			MIG_INFO(USER_LEVEL,"Redis Conntions error %s",str);
		++it;
	}
    return true;
}

bool RedisStorageEngineImpl::Release(){
	RedisClose(c_);
	return true;
}

bool RedisStorageEngineImpl::SetValue(const char* key,const size_t key_len,
				                      const char* val,const size_t val_len){

    if(PingRedis()!=1)
    	return false;
	int ret = RedisAddValue(c_,key,key_len,val,val_len);
	//MIG_DEBUG(USER_LEVEL,"=====ret[%d]=======",ret);
    return (ret==1?true:false);
}

bool RedisStorageEngineImpl::AddValue(const char* key,const size_t key_len,
				                      const char* val,const size_t val_len){

    int r = 0;
    char* cval;
    size_t cval_len;
    
    if(PingRedis()!=1)
    	return false;
    	
//     r = RedisGetValue(c_,key,key_len,&cval,&cval_len);
//     if(r){
//     	if(cval) delete cval;
//     	return false;
//     }
    return SetValue(key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::ReplaceValue(const char* key,const size_t key_len,
					                      const char* val,const size_t val_len){
    
    if(PingRedis()!=1)
    	return false;
    return AddValue(key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::GetValue(const char* key,const size_t key_len,
				                      char**val,size_t* val_len){

    if(PingRedis()!=1)
    	return false;
	MIG_DEBUG(USER_LEVEL,"key[%s] key_len[%d]",key,key_len);
    return RedisGetValue(c_,key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::DelValue(const char* key,const size_t key_len){

    if(PingRedis()!=1)
    	return false;
    return RedisDelValue(c_,key,key_len)==1?true:false;
}

bool RedisStorageEngineImpl::MGetValue(const char* const * key_array,
	                                   const size_t* key_len_array,
	                                   size_t element_count){

    return true;
}

bool RedisStorageEngineImpl::FetchValue(char* key,size_t *key_len,
	                                    char** value,size_t *val_len){
   
   return true;
}

bool RedisStorageEngineImpl::IncDecValue(const char* key, size_t key_len, int64 incby,
		int64& result) {
    if(PingRedis()!=1)
    	return false;
	MIG_DEBUG(USER_LEVEL,"key[%s] key_len[%d]",key,key_len);
    return RedisIncDecValue(c_, key, key_len, incby, (long long *)&result)==1?true:false;
}

bool RedisStorageEngineImpl::PingRedis(){
	
	return (RedisPingRedis(c_)==1)?true:false;
}

bool RedisStorageEngineImpl::AddHashRadomElement(const char* hash_name,
												 const char* val,const size_t val_len){

	 if(PingRedis()!=1)
		 return false;
	 int64 current_pos = RedishHashSize(c_,hash_name);
	 //MIG_DEBUG(USER_LEVEL,"current pos[%lld]",current_pos);
	 std::stringstream index;
	 index<<current_pos;

	 bool r = RedisAddHashElement(c_,hash_name,index.str().c_str(),index.str().length(),
							val,val_len);
	 return r;
}

bool RedisStorageEngineImpl::GetHashRadomElement(const char* hash_name,char** val,
												 size_t *val_len,const int radom_num){
	int32 current_size = RedishHashSize(c_,hash_name);
	std::stringstream index;
	bool r = false;
	if (current_size>0){
		//time_t current_time = time(NULL);
		int tindex= (radom_num)%(current_size);
		tindex=(tindex>0)?tindex:(0-tindex);
		index<<((tindex));
		MIG_DEBUG(USER_LEVEL,"index[%s] hashname[%s] radom_num[%lld] current[%d]",
				 index.str().c_str(),hash_name,
				 radom_num,current_size);
		if(PingRedis()!=1)
			return false;
		r = RedisGetHashElement(c_,hash_name,index.str().c_str(),
									index.str().length(),val,val_len);
	}
	return r;
}

bool RedisStorageEngineImpl::DelHashRadomElement(const char* hash_name){
	if(PingRedis()!=1)
		return false;
	return RedisDelHash(c_,hash_name);
}

bool RedisStorageEngineImpl::AddHashElement(const char* hash_name,const char* key,
											const size_t key_len, const char* val,
											const size_t val_len){
	if(PingRedis()!=1)
		return false;
    return RedisAddHashElement(c_,hash_name,key,key_len,val,val_len);
}

bool RedisStorageEngineImpl::SetHashElement(const char* hash_name,
		const char* key, const size_t key_len, const char* val,
		const size_t val_len) {
	if(PingRedis()!=1)
		return false;
    return RedisSetHashElement(c_,hash_name,key,key_len,val,val_len);
}

bool RedisStorageEngineImpl::GetHashElement(const char* hash_name,const char* key,
											const size_t key_len, char** val,size_t *val_len){
	if(PingRedis()!=1)
		return false;
    return RedisGetHashElement(c_,hash_name,key,key_len,val,val_len);
}

bool RedisStorageEngineImpl::DelHashElement(const char* hash_name,const char* key,const size_t key_len){
	if(PingRedis()!=1)
		return false;
	return RedisDelHashElement(c_,hash_name,key,key_len);
}

bool RedisStorageEngineImpl::AddListElement(const char* key,const size_t key_len,
	                                        const char* val,const size_t val_len){
	if(PingRedis()!=1)
		return false;
	return RedisAddListElement(c_,key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::GetListElement (const char* key,const size_t key_len,
	                                         const int index,char** val,size_t *val_len){
    if(PingRedis()!=1)
    	return false;
    	
	return RedisGetListElement(c_,key,key_len,index,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::DelListElement(const char* key,const size_t key_len,
	                                        const int index){
	if(PingRedis()!=1)
    	return false;
	return RedisDelListElement(c_,index,key,key_len)==1?true:false;
}

bool RedisStorageEngineImpl::SetListElement(const int index,const char* key,
	                                        const size_t key_len,const char* val,
	                                        const size_t val_len){
	 if(PingRedis()!=1)
    	return false;
	return RedisSetListElement(c_,index,key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::GetHashValues(const char* hash_name,const size_t hash_name_len,
										   std::list<std::string>& list){
    int r = 0;
	if (PingRedis()!=1)
		return false;
	char** pptr = NULL;
	int n;
	warrper_redis_reply_t*  rp = NULL;
	rp = RedisGetHashValueAll(c_, hash_name,hash_name_len,&pptr,&n);
	for(r =0;r<n;r++){
		std::string str;
		str.assign(pptr[r]);
		list.push_back(str);
	}
	free(*pptr);
	if(rp==NULL)
		return false;
	RedisFreeReply(rp);
	return true;
}

bool RedisStorageEngineImpl::GetListAll(const char* key,const size_t key_len,
	                                    std::list<std::string>& list){

    int r = 0;
	if(PingRedis()!=1)
    	return false;
    char** pptr = NULL;
    int n;
	warrper_redis_reply_t*  rp = NULL;
	rp = RedisGetListAll(c_, key,key_len,&pptr,&n);
	for(r =0;r<n;r++){
		std::string str;
		str.assign(pptr[r]);
		list.push_back(str);
	}
	free(*pptr);
	if(rp==NULL)
		return false;
	RedisFreeReply(rp);
	return true;
}

bool RedisStorageEngineImpl::GetListRange(const char* key, const size_t key_len,
		int from, int to, std::list<std::string>& list) {
    int i = 0;
	if(PingRedis()!=1)
    	return false;

    char** pptr = NULL;
    int n;
	warrper_redis_reply_t*  rp = RedisGetListRange(c_, key,key_len,from,to,&pptr,&n);
	if (NULL == rp)
		return false;
	for(i =0;i<n;i++){
		std::string str;
		str.assign(pptr[i]);
		list.push_back(str);
		free(pptr[i]);
	}
	RedisFreeReply(rp);
	return true;
}

}
