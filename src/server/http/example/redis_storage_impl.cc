#include "redis_storage_impl.h"
#include <stdio.h>
#include <stdlib.h>
#include "mig_log.h"

RedisStorageEngineImpl::RedisStorageEngineImpl(){

}

RedisStorageEngineImpl::~RedisStorageEngineImpl(){

}

bool RedisStorageEngineImpl::Connections(std::list<ConnAddr>& addrlist){
	ConnAddr  addr;
	char* str;
	std::list<ConnAddr>::iterator it = addrlist.begin();
	while(it!=addrlist.end()){
		addr = (*it);
		str = RedisConnections(&c_,addr.host().c_str(),addr.port());
		if(str!=NULL)
			MIG_INFO(USER_LEVEL,"Redis Conntions error %s",str);
	}
    return true;
}

bool RedisStorageEngineImpl::Release(){
	
	return true;
}

bool RedisStorageEngineImpl::SetValue(const char* key,const size_t key_len,
				const char* val,const size_t val_len,struct BaseStorage* base){

    if(PingRedis()!=1)
    	return false;
    return (RedisAddValue(c_,key,key_len,val,val_len)==1?true:false);
}

bool RedisStorageEngineImpl::AddValue(const char* key,const size_t key_len,
				const char* val,const size_t val_len,struct BaseStorage* base){

    int r = 0;
    char* cval;
    size_t cval_len;
    
    if(PingRedis()!=1)
    	return false;
    	
    r = RedisGetValue(c_,key,key_len,&cval,&cval_len);
    if(r){
    	if(cval) delete cval;
    	return false;
    }
    return SetValue(key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::ReplaceValue(const char* key,const size_t key_len,
					const char* val,const size_t val_len,struct BaseStorage* base){
    
    if(PingRedis()!=1)
    	return false;
    return AddValue(key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::GetValue(const char* key,const size_t key_len,
				     char**val,size_t* val_len,struct BaseStorage** base){

    if(PingRedis()!=1)
    	return false;
    return RedisGetValue(c_,key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::DelValue(const char* key,const size_t key_len){

    if(PingRedis()!=1)
    	return false;
    return RedisDelValue(c_,key,key_len)==1?true:false;
}

bool RedisStorageEngineImpl::MGetValue(const char* const * key_array,
		const size_t* key_len_array,size_t element_count){

    return true;
}

bool RedisStorageEngineImpl::FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len){
   
   return true;
}

bool RedisStorageEngineImpl::PingRedis(){
	
	return (RedisPingRedis(c_)==1)?true:false;
}


bool RedisStorageEngineImpl::AddListElement(const char* key,const size_t key_len,const char* val,const size_t val_len){
	if(PingRedis()!=1)
		return false;
	return RedisAddListElement(c_,key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::GetListElement (const char* key,const size_t key_len,const int index,char** val,size_t *val_len){
    if(PingRedis()!=1)
    	return false;
    	
	return RedisGetListElement(c_,key,key_len,index,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::DelListElement(const char* key,const size_t key_len,const int index){
	if(PingRedis()!=1)
    	return false;
	return RedisDelListElement(c_,index,key,key_len)==1?true:false;
}

bool RedisStorageEngineImpl::SetListElement(const int index,const char* key,const size_t key_len,const char* val,const size_t val_len){
	 if(PingRedis()!=1)
    	return false;
	return RedisSetListElement(c_,index,key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::GetListAll(const char* key,const size_t key_len,std::list<std::string>& list){

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
	if(rp==NULL)
		return false;
	RedisFreeReply(rp);
	return true;
}