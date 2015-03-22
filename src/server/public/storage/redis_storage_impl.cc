#include "redis_storage_impl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "log/mig_log.h"
#include "hiredis.h"

namespace base_storage{
	
RedisStorageEngineImpl::RedisStorageEngineImpl(){
	Init();
}

RedisStorageEngineImpl::~RedisStorageEngineImpl(){
	Release();
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
		if(str!=NULL){ //���Ӵ��� ֪ͨ���ߵ�
			MIG_INFO(USER_LEVEL,"Redis Conntions error %s",str);
			return false;
		}
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

bool RedisStorageEngineImpl::IncrValue(const char* key,const size_t key_len, 
									   const char* val,const size_t val_len){
	bool r = false;
	char* default_value = "1";
	size_t default_size = 1;
	if (PingRedis()!=1)
		return false;


	if((ReidsIsExist(c_,key,key_len))==1)
		return RedisIncr(c_,key,key_len);
	else
		return RedisAddValue(c_,key,key_len,default_value,default_size);
}

bool RedisStorageEngineImpl::DecrValue(const char* key,const size_t key_len, 
									   const char* val,const size_t val_len){
	char** sval;
	size_t* sval_len;
	if (PingRedis()!=1)
		return false;

	if((ReidsIsExist(c_,key,key_len))==1)
	    return RedisDecr(c_,key,key_len);
	return false;
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

int RedisStorageEngineImpl::GetHashSize(const char* hash_name){

	if (PingRedis()!=1)
		return 0;
	return RedishHashSize(c_,hash_name);
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

bool RedisStorageEngineImpl::AddListElement(const char* key,
											const size_t key_len,
	                                        const char* val,
											const size_t val_len,
											const int flag){
	if(PingRedis()!=1)
		return false;
	return RedisAddListElement(c_,key,key_len,val,val_len,flag)==1?true:false;
}

bool RedisStorageEngineImpl::PopListElement(const char* key,const size_t key_len,char** val,
                		size_t *val_len,const int flag){
	if(PingRedis()!=1)
		return false;
	return RedisPopListElement(c_,key,key_len,val,val_len,flag)==1?true:false;
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

int RedisStorageEngineImpl::GetListSize(const char *list_name){
	if (PingRedis()!=1)
		return false;
	return ReidsGetListSize(c_,list_name,strlen(list_name)+1);
}

bool RedisStorageEngineImpl::SetListElement(const int index,const char* key,
	                                        const size_t key_len,const char* val,
	                                        const size_t val_len){
	 if(PingRedis()!=1)
    	return false;
	return RedisSetListElement(c_,index,key,key_len,val,val_len)==1?true:false;
}

bool RedisStorageEngineImpl::GetAllHash(const char* hash_name, 
										const size_t hash_name_len, 
							std::map<std::string,std::string>& map){
   int r = 0;
   if (PingRedis()!=1)
	   return false;
   char** pptr = NULL;
   int n;
   warrper_redis_reply_t* rp = NULL;
   rp = RedisGetAllHash(c_,hash_name,hash_name_len,&pptr,&n);
   for (r = 0; r<(n/2); r++){
	   std::string key;
	   std::string value;
	   if ((pptr[r]==NULL)||(pptr[r+1]==NULL))
		   continue;
	   key.assign(pptr[r]);
	   value.assign(pptr[r+1]);
	   map[key] = value;
   }
   free(pptr);
   if(rp==NULL)
	   return false;
   RedisFreeReply(rp);
   return true;
}

bool RedisStorageEngineImpl::GetHashValues(const char* hash_name,
										   const size_t hash_name_len,
										   std::list<std::string>& list){
    int r = 0;
	if (PingRedis()!=1)
		return false;
	char** pptr = NULL;
	int n = 0;
	warrper_redis_reply_t*  rp = NULL;
	rp = RedisGetHashValueAll(c_, hash_name,hash_name_len,&pptr,&n);
	if(n == 0 || pptr == NULL)
		return false;
	for(r =0;r<n;r++){
		std::string str;
		str.assign(pptr[r]);
		list.push_back(str);
	}
	free(pptr);
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
    int n = 0;
	warrper_redis_reply_t*  rp = NULL;
	rp = RedisGetListAll(c_, key,key_len,&pptr,&n);
	if(n == 0 || pptr == NULL)
		return false;
	for(r =0;r<n;r++){
		std::string str;
		str.assign(pptr[r]);
		list.push_back(str);
	}
	free(pptr);
	if(rp==NULL)
		return false;
	RedisFreeReply(rp);
	return true;
}

bool RedisStorageEngineImpl::GetListRange(const char* key, 
										  const size_t key_len,
		                                  int from, int to, 
										  std::list<std::string>& list,
										  const int flag) {
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
	}
	free(pptr);
	RedisFreeReply(rp);
	return true;
}

CommandReply *RedisStorageEngineImpl::DoCommand(const char *format/*, ...*/) {
	//va_list ap;
	//va_start(ap, format);
	//warrper_redis_reply_t *rp = RedisDoCommandV(c_, format, ap);
	warrper_redis_reply_t *rp = RedisDoCommand(c_, format);
	//va_end(ap);
	if (NULL == rp)
		return false;

	CommandReply *reply = _CreateReply(rp->reply);
	RedisFreeReply(rp);
	return reply;
}

CommandReply* RedisStorageEngineImpl::_CreateReply(redisReply* reply) {
	switch (reply->type) {
	case REDIS_REPLY_ERROR:
		return new ErrorReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_NIL:
		return new CommandReply(CommandReply::REPLY_NIL);
	case REDIS_REPLY_STATUS:
		return new StatusReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_INTEGER:
		return new IntegerReply(reply->integer);
	case REDIS_REPLY_STRING:
		return new StringReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_ARRAY: {
		ArrayReply *rep = new ArrayReply();
		for (size_t i = 0; i < reply->elements; ++i) {
			if (CommandReply *cr = _CreateReply(reply->element[i]))
				rep->value.push_back(cr);
		}
		return rep;
	}
	default:
		break;
	}
	return NULL;
}

}
