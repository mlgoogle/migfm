#ifndef _MIG_FM_REDIS_WARRPER_H__
#define _MIG_FM_REDIS_WARRPER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hiredis.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct warrper_redis_context_t {
    redisContext*	context;
}warrper_redis_context_t;

typedef struct warrper_redis_reply_t{
	redisReply* reply;
}warrper_redis_reply_t;



char* RedisConnections(warrper_redis_context_t** context ,
					const char* host,const int port);

int RedisAddValue(warrper_redis_context_t* context,
				const char* key,const size_t key_len,
				const char* val,const size_t val_len);

int RedisGetValue(warrper_redis_context_t* context,
				const char* key,const size_t key_len,
				char** val,size_t *val_len);
						
int RedisDelValue(warrper_redis_context_t* context,
			   const char* key,const size_t key_len);
							
int RedisPingRedis(warrper_redis_context_t* context);


int RedisAddHashElement(warrper_redis_context_t* context,const char* hash_name,
						const char* key,const size_t key_len,
						const char* val,const size_t val_len);

int RedisGetHashElement(warrper_redis_context_t* context,const char* hash_name,
						const char* key,const size_t key_len,
						char** val,size_t* val_len);

int RedisDelHashElement(warrper_redis_context_t* context,const char* hash_name,
						const char* key,const size_t key_len);

long long RedishHashSize(warrper_redis_context_t* context,const char* hash_name);

int RedisDelHash(warrper_redis_context_t* context,const char* hash_name);

int RedisAddListElement(warrper_redis_context_t* context,
				const char* key,const size_t key_len,
				const char* val,const size_t val_len);
				

int RedisGetListElement(warrper_redis_context_t* context,
				const char* key,const size_t key_len,int index,
				char** val,size_t *val_len);
			
int RedisDelListElement(warrper_redis_context_t* context,int index,
			   const char* key,const size_t key_len);
			  
int RedisSetListElement(warrper_redis_context_t* context,int index,
					const char* key,const size_t key_len,
					const char* val,const size_t val_len);

long long ReidsGetListSize(warrper_redis_context_t* context,const char* key,
					  const size_t key_len);

warrper_redis_reply_t* RedisGetListAll(warrper_redis_context_t* context,const char* key,const size_t key_len,char***val,int* val_len);
							
int RedisFreeReply(warrper_redis_reply_t* wa);

#ifdef __cplusplus
}
#endif
#endif

