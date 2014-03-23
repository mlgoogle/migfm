#ifndef MIG_FM_MYSQL_STORAGE_IMPL_H__
#define MIG_FM_MYSQL_STORAGE_IMPL_H__
#include <list>
#include <string>
#include <sstream>
#include "mysql.h"
#include "conn_addr.h"
#include "storage.h"
#define TABLE_USERINFO  "migfm_userinfo"
class MysqlDB{
public:
	MysqlDB();
	virtual ~MysqlDB();
	bool Init();
public:
	bool Connect(const std::string& host,const int port,
				const std::string& user,const std::string& passwd,
				const std::string& db);
	bool SqlExec(std::string& sql);
	bool AffectedRows(unsigned long& rows);
	bool SqlClose();
	bool RecordCount(unsigned long& count);
	bool FetchRows(MYSQL_ROW& row);
	bool Close();
	bool CheckConnect(void);
public:
	MYSQL mysql_;
	MYSQL_RES* result_;
	MYSQL_ROW rows_;
	bool connected_;

};


class MysqlLogic{
public:
	MysqlLogic(){};
	~MysqlLogic(){};

    bool Connections(std::list<ConnAddr>& addrlist);
    	
    bool Release();
    
    bool AddUserInfo(const std::string& id,const std::string& usrname,const std::string& password,
					 const int sex,const int address,const std::string& name,
					 const std::string& id_cards,const std::string& head);
private:
    MysqlDB             mysql_;
};

class MysqlStorageEngineImpl: public StorageEngine{

public:
    MysqlStorageEngineImpl();
    virtual ~MysqlStorageEngineImpl();

    virtual bool Connections(std::list<ConnAddr>& addrlist);
 
    virtual bool Release();
   
    virtual bool SetValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL);

    virtual bool AddValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL);
  
    virtual bool ReplaceValue(const char* key,const size_t key_len,const char* val,const size_t val_len,struct BaseStorage* base=NULL);

    virtual bool GetValue(const char* key,const size_t key_len,char** val,size_t* val_len,struct BaseStorage** base=NULL);

    virtual bool DelValue(const char* key,const size_t key_len);
  
    virtual bool MGetValue(const char* const * key_array,const size_t *key_len_array,size_t element_count);

    virtual bool FetchValue(const char* key,size_t *key_len,char** value,size_t *val_len);
    
    
        //list

    virtual bool AddListElement(const char* key,const size_t key_len,const char* val,const size_t val_len);
    
    virtual bool GetListElement (const char* key,const size_t key_len,const int index,char** val,size_t *val_len);
    
    virtual bool DelListElement(const char* key,const size_t key_len,const int index);
    
    virtual bool SetListElement(const int index,const char* key,const size_t key_len,const char* val,const size_t val_len);
    
    virtual bool GetListAll(const char* key,const size_t key_len,std::list<std::string>& list);

private:
	MysqlLogic     logic_;
};
#endif
