#ifndef MIG_FM_PUBLIC_STORAGE_STORAGE_H__
#define MIG_FM_PUBLIC_STORAGE_STORAGE_H__
#include <string>
#include <list>
#include "basic/basictypes.h"
#include "basic/basic_info.h"

namespace base_storage{

enum StorageImplType{
	IMPL_DIC = 0,
	IMPL_DB = 1
};

enum DictionaryImplType{
   IMPL_MEM = 0,
   IMPL_RADIES = 1
};

enum DBImplType{
	IMPL_MYSQL = 0,
	IMPL_MSSQL = 1
};

typedef struct db_conn_t{
    void *proc;
}db_conn_t;


typedef struct db_res_t{
    void *proc;
}db_res_t;

typedef struct db_row_t{
    void *proc;
}db_row_t;

class StorageEngine{
public:
	static StorageEngine* Create(int32 type);
public:
	virtual bool Connections(std::list<base::ConnAddr>& addrlist) = 0;
	virtual bool Release() = 0;
};

class DBStorageEngine:public StorageEngine{
public:
	static DBStorageEngine* Create(int32 type);
	virtual ~DBStorageEngine(){}

public:
	virtual bool Release() = 0;
	
	virtual bool SQLExec(const char* sql) = 0;
	
	virtual bool Affected(unsigned long& rows) = 0;
	
	virtual uint32 RecordCount() = 0;
	
    virtual bool FreeRes() = 0;
    
	virtual db_row_t* FetchRows(void) = 0;//get Recordset
	
	virtual bool CheckConnect(void) =0;
	
};

class DictionaryStorageEngine:public StorageEngine{

public:
    static DictionaryStorageEngine* Create(int32 type);
    virtual ~DictionaryStorageEngine(){}

public:
	
    virtual bool  Release() = 0;
 
    virtual bool  SetValue(const char* key,const size_t key_len,
    						const char* val,const size_t val_len) = 0;

    virtual bool  AddValue(const char* key,const size_t key_len,
    						const char* val,const size_t val_len) = 0;

    virtual bool  ReplaceValue(const char* key,const size_t key_len,
    							const char* val,const size_t val_len) = 0;

    virtual bool  GetValue(const char* key,const size_t key_len,
    						char** val,size_t* val_len) = 0;

    virtual bool  DelValue(const char* key,const size_t key_len) = 0;

    virtual bool  MGetValue(const char* const * key_array,const size_t *key_len_array,
    						size_t element_count) = 0;

    virtual bool FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len) = 0;
    
        //list
    virtual bool AddListElement(const char* key,const size_t key_len,
    								const char* val,const size_t val_len) = 0;
    
    virtual bool GetListElement (const char* key,const size_t key_len,
    								const int index,char** val,size_t *val_len) = 0;
    
    virtual bool DelListElement(const char* key,const size_t key_len,const int index) = 0;
    
    virtual bool SetListElement(const int index,const char* key,const size_t key_len,
    								const char* val,const size_t val_len) =0 ;
    
	virtual bool GetListAll(const char* key,const size_t key_len,std::list<std::string>& list) = 0;
};


}
#endif
