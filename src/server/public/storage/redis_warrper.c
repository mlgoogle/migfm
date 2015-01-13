#include "redis_warrper.h"
char* RedisConnections(warrper_redis_context_t** context ,
				const char* host,const int port){
	struct timeval timeout = {1,500000};
	(*context)->context = redisConnectWithTimeout(host,port,timeout);
	if((*context)->context->err)
		return (*context)->context->errstr;
	return NULL;
}

int RedisAddValue(warrper_redis_context_t* context,
				const char* key,const size_t key_len,
				const char* val,const size_t val_len){
     
     redisReply* reply;
     reply = redisCommand(context->context,"SET %b %b",key,key_len,val,val_len);
//      if(strcmp(reply->str,val)==0){
//      	freeReplyObject(reply);
//      	return 1;
//      }else{
     	freeReplyObject(reply);
//      	return 0;
//      }
	return 1;
}

int RedisIncr(warrper_redis_context_t* context,const char* key,const size_t key_len){
	redisReply* reply;
	reply = redisCommand(context->context,"incr %b",key,key_len);
	freeReplyObject(reply);
	return 1;
}

int RedisDecr(warrper_redis_context_t* context, const char* key,const size_t key_len){
	redisReply* reply;
	reply = redisCommand(context->context,"decr %b",key,key_len);
	freeReplyObject(reply);
	return 1;
}

int ReidsIsExist(warrper_redis_context_t* context,
				 const char* key,const size_t key_len){
	redisReply* reply;
	reply = redisCommand(context->context,"GET %s",key);
	if(reply->len!=0){
	 if (strlen(reply->str)<=0){
		 printf("########%d#########",strlen(reply->str));
		 return 0;
	 }
	 freeReplyObject(reply);
	 return 1;
	}
	freeReplyObject(reply);
	return 0;
}

int RedisGetValue(warrper_redis_context_t* context,
				const char* key,const size_t key_len,
				char** val,size_t *val_len){

	redisReply* reply;
	reply = redisCommand(context->context,"GET %s",key);
	if(reply->len!=0){
		if (strlen(reply->str)<=0){
			printf("########%d#########",strlen(reply->str));
			return 0;
		}

		*val = (char*)malloc(reply->len+1);
		memcpy(*val,reply->str,reply->len);
		(*val_len) = reply->len+1;
		freeReplyObject(reply);
		return 1;
	}
	*val_len = 0;
	freeReplyObject(reply);
	return 0;
}

int RedisDelValue(warrper_redis_context_t* context,
			   const char* key,const size_t key_len){
	redisReply* reply;
	reply = redisCommand(context->context,"DEL %s",key);
	freeReplyObject(reply);
	return 1;			 
}

int RedisIncDecValue(warrper_redis_context_t* context, const char* key,
		size_t key_len, long long incby, long long *result) {
	redisReply* reply;
	char *val;
	size_t val_len;
	int ret;

	if (0 == incby) {
		ret = RedisGetValue(context, key, key_len, &val, &val_len);
		if (NULL != val) {
			if (NULL != result)
				*result = strtoll(val, NULL, 10);
			free(val);
		}
		return ret;
	}

	if (1 == incby)
		reply = redisCommand(context->context,"INCR %s",key);
	else if (-1 == incby)
		reply = redisCommand(context->context,"DECR %s",key);
	else if (incby > 0)
		reply = redisCommand(context->context,"INCRBY %s %lld", key, incby);
	else if (incby < 0)
		reply = redisCommand(context->context,"DECRBY %s %lld", key, incby);

	if (result)
		*result = reply->integer;

	freeReplyObject(reply);
	return 1;
}

int RedisPingRedis(warrper_redis_context_t* context){
	redisReply* reply;
	reply = redisCommand(context->context,"PING");
	if(reply==NULL)
		return 0;
	if(strcmp(reply->str,"PONG")==0){
		freeReplyObject(reply);
		return 1;
	}else{
		freeReplyObject(reply);
		return 0;
	}
}


int RedisAddHashElement(warrper_redis_context_t* context,const char* hash_name, 
						const char* key,const size_t key_len, 
						const char* val,const size_t val_len){
	redisReply* reply;
	reply = redisCommand(context->context,"hsetnx %s %s %s",hash_name,
							key,val);
	freeReplyObject(reply);
	return 1;
} 

int RedisSetHashElement(warrper_redis_context_t* context, const char* hash_name,
		const char* key, const size_t key_len, const char* val,
		const size_t val_len) {
	redisReply* reply;
	reply = redisCommand(context->context,"hset %s %s %s",hash_name,
							key,val);
	freeReplyObject(reply);
	return 1;
}

int RedisGetHashElement(warrper_redis_context_t* context,const char* hash_name, 
						const char* key,const size_t key_len, char** val,size_t* val_len){
	redisReply* reply;
	reply =  redisCommand(context->context,"hget %s %s",hash_name,key);
	if(reply->len!=0){
		if (strlen(reply->str)<=0){
			printf("########%d#########",strlen(reply->str));
			return 0;
		}
		*val = (char*)malloc(reply->len+1);
		memcpy(*val,reply->str,reply->len+1);
		(*val_len) = reply->len+1;
		freeReplyObject(reply);
		return 1;
	}
	*val_len = 0;
	freeReplyObject(reply);
	return 0;
}





int RedisDelHashElement(warrper_redis_context_t* context,const char* hash_name, 
						const char* key,const size_t key_len){
	redisReply* reply;
	reply = redisCommand(context->context,"hdel %s %s",hash_name,key);
	freeReplyObject(reply);
	return 1;
}



long long RedishHashSize(warrper_redis_context_t* context,const char* hash_name){
	redisReply* reply;
	long long hash_name_size;
	char* val = NULL;
	reply = redisCommand(context->context,"hlen %s",hash_name);
    hash_name_size = reply->integer;
	freeReplyObject(reply);
	return hash_name_size;
}



int RedisDelHash(warrper_redis_context_t* context,const char* hash_name){
	redisReply* reply;
	reply = redisCommand(context->context,"hdel %s",hash_name);
	freeReplyObject(reply);
	return 1;
}

int RedisAddListElement(warrper_redis_context_t* context,
				const char* key,const size_t key_len,
				const char* val,const size_t val_len,
				const int flag){
	redisReply* reply;
	if (flag==0)

		reply = redisCommand(context->context,"RPUSH %s %s",key,val);
	else
		reply = redisCommand(context->context,"LPUSH %s %s",key,val);

	freeReplyObject(reply);
	return 1;
}

int RedisGetListElement(warrper_redis_context_t* context,
				const char* key,const size_t key_len,int index,
				char** val,size_t *val_len){
	redisReply* reply;
	reply =  redisCommand(context->context,"LINDEX %s %d",key,index);
// 	*val = (char*)malloc(strlen(reply->str)+1);
// 	memcpy(*val,reply->str,strlen(reply->str));
// 	*val[strlen(reply->str)] = '\0';
	freeReplyObject(reply);
	return 1;
}

int RedisPopListElement(warrper_redis_context_t* context,
		   const char* key,const size_t key_len,char** val,size_t *val_len,
		   const int flag){
	redisReply* reply;
	if (flag==0)
		reply =  redisCommand(context->context,"RPOP %s",key);
	else
		reply =  redisCommand(context->context,"LPOP %s",key);
	if(reply->len!=0){
		if (strlen(reply->str)<=0){
			printf("########%d#########",strlen(reply->str));
			return 0;
		}
		*val = (char*)malloc(reply->len+1);
		memcpy(*val,reply->str,reply->len+1);
		(*val_len) = reply->len+1;
		freeReplyObject(reply);
		return 1;
	}
	*val_len = 0;
	freeReplyObject(reply);
	return 0;
}

int RedisSetListElement(warrper_redis_context_t* context,int index,
					const char* key,const size_t key_len,
					const char* val,const size_t val_len){
	redisReply* reply;
	reply = redisCommand(context->context,"LSET %s %d %s",key,index,val);
	freeReplyObject(reply);
	return 1;
}

int RedisDelListElement(warrper_redis_context_t* context,int index,
			   const char* key,const size_t key_len){
	redisReply* reply;
	reply = redisCommand(context->context,"RPOP %s",key);
	freeReplyObject(reply);
	return 1;
}

warrper_redis_reply_t* RedisGetAllHash(warrper_redis_context_t* context, 
									   const char* key,const size_t key_len, 
									   char***val,int* val_len){
	redisReply* reply;
	int j =0;
	warrper_redis_reply_t* wa_re = NULL;
	reply = redisCommand(context->context,"hgetall %s",key);
	if(reply->type==REDIS_REPLY_ARRAY){
	   wa_re = (warrper_redis_reply_t*)malloc(sizeof(warrper_redis_reply_t));
	   wa_re->reply = reply;
	   (*val_len) = wa_re->reply->elements;
	   (*val) = (char**)malloc(sizeof(char*)*(wa_re->reply->elements));
	   for(j =0;j<reply->elements;j++){
		   (*val)[j] = reply->element[j]->str;
	   }
	   return wa_re;
	}
	return NULL;
}

warrper_redis_reply_t* RedisGetHashValueAll(warrper_redis_context_t* context,
											const char* key,const size_t key_len,
											char***val,int* val_len){
	redisReply* reply;
	int j =0;
	warrper_redis_reply_t* wa_re = NULL;
	reply = redisCommand(context->context,"hvals %s",key);
	if(reply->type==REDIS_REPLY_ARRAY){
	   wa_re = (warrper_redis_reply_t*)malloc(sizeof(warrper_redis_reply_t));
	   wa_re->reply = reply;
	   (*val_len) = wa_re->reply->elements;
	   (*val) = (char**)malloc(sizeof(char*)*(wa_re->reply->elements));
	   for(j =0;j<reply->elements;j++){
		   (*val)[j] = reply->element[j]->str;
	   }
	   return wa_re;
	}
	return NULL;
}

warrper_redis_reply_t* RedisGetListAll(warrper_redis_context_t* context,
					const char* key,const size_t key_len,char***val,
					int* val_len){
	redisReply* reply;
	int j =0;
	warrper_redis_reply_t* wa_re = NULL;
	reply = redisCommand(context->context,"LRANGE %s 0 -1",key);
	if(reply->type==REDIS_REPLY_ARRAY){
		wa_re = (warrper_redis_reply_t*)malloc(sizeof(warrper_redis_reply_t));
		wa_re->reply = reply;
		(*val_len) = wa_re->reply->elements;
		(*val) = (char**)malloc(sizeof(char*)*(wa_re->reply->elements));
		for(j =0;j<reply->elements;j++){
			(*val)[j] = reply->element[j]->str;
		}
		return wa_re;
	}
	return NULL;
}

int RedisFreeReply(warrper_redis_reply_t* wa){
	freeReplyObject(wa->reply);
	free(wa);
	return 0;
}

long long ReidsGetListSize(warrper_redis_context_t* context,
					 const char* key, const size_t key_len){
	 redisReply* reply;
	 long long hash_name_size;
	 char* val = NULL;
	 reply =  redisCommand(context->context,"llen %s",key);
	 hash_name_size = reply->integer;
	 freeReplyObject(reply);
	 return hash_name_size;
}

int RedisClose(warrper_redis_context_t* context){
	redisFree(context->context);
}

warrper_redis_reply_t *RedisGetListRange(warrper_redis_context_t* context, const char* key,
		const size_t key_len, int from, int to, char*** val, int* val_len) {
	redisReply* reply;
	int j =0;
	warrper_redis_reply_t* wa_re = NULL;
	reply = redisCommand(context->context,"LRANGE %s %d %d", key, from, to);
	if(reply->type==REDIS_REPLY_ARRAY){
		wa_re = (warrper_redis_reply_t*)malloc(sizeof(warrper_redis_reply_t));
		wa_re->reply = reply;
		(*val_len) = wa_re->reply->elements;
		(*val) = (char**)malloc(sizeof(char*)*(wa_re->reply->elements));
		for(j =0;j<reply->elements;j++){
			(*val)[j] = reply->element[j]->str;
		}
		return wa_re;
	}
	freeReplyObject(reply);
	return NULL;
}

warrper_redis_reply_t* RedisDoCommand(warrper_redis_context_t* context,
		const char* format/*, ...*/) {
	redisReply *reply;
	warrper_redis_reply_t *wa_re;
	//va_list ap;

	//va_start(ap, format);
	reply = redisCommand(context, format/*, ap*/);
	//va_end(ap);

	if (NULL == reply)
		return NULL;

	wa_re = (warrper_redis_reply_t*)malloc(sizeof(warrper_redis_reply_t));
	wa_re->reply = reply;
	return wa_re;
}

warrper_redis_reply_t* RedisDoCommandV(warrper_redis_context_t* context,
		const char *format, va_list ap) {
	redisReply *reply;
	warrper_redis_reply_t *wa_re;

	reply = redisvCommand(context, format, ap);

	if (NULL == reply)
		return NULL;

	wa_re = (warrper_redis_reply_t*)malloc(sizeof(warrper_redis_reply_t));
	wa_re->reply = reply;
	return wa_re;
}
