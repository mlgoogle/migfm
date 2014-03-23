#ifndef MIG_FM_STORAGE_H__
#define MIG_FM_STORAGE_H__
#include <string>
#include <list>
#include "conn_addr.h"
enum ImplType{
   IMPL_MEM = 0,
   IMPL_RADIES = 1,
   IMPL_MYSQL = 2
};

struct BaseStorage{
	int opcode_;
};

class StorageEngine{

public:
    static StorageEngine* Create(int type);
    virtual ~StorageEngine(){}

public:
    virtual bool  Connections(std::list<ConnAddr>& addrlist) = 0;
   
    virtual bool  Release() = 0;
 
    virtual bool  SetValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL) = 0;

    virtual bool  AddValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL) = 0;

    virtual bool  ReplaceValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL) = 0;

    virtual bool  GetValue(const char* key,const size_t key_len,char** val,size_t* val_len,struct BaseStorage** base=NULL) = 0;

    virtual bool  DelValue(const char* key,const size_t key_len) = 0;

    virtual bool  MGetValue(const char* const * key_array,const size_t *key_len_array,size_t element_count) = 0;

    virtual bool FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len) = 0;
    
        //list
    virtual bool AddListElement(const char* key,const size_t key_len,const char* val,const size_t val_len) = 0;
    
    virtual bool GetListElement (const char* key,const size_t key_len,const int index,char** val,size_t *val_len) = 0;
    
    virtual bool DelListElement(const char* key,const size_t key_len,const int index) = 0;
    
    virtual bool SetListElement(const int index,const char* key,const size_t key_len,const char* val,const size_t val_len) =0 ;
    
	virtual bool GetListAll(const char* key,const size_t key_len,std::list<std::string>& list) = 0;
};
#endif
