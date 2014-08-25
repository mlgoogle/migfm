#ifndef _MIG_FM_PUBLIC_STORAGE_MYSQL_STORAGE_IMPL_H__
#define _MIG_FM_PUBLIC_STORAGE_MYSQL_STROAGE_IMPL_H__
#include <list>
#include "basic/basic_info.h"
#include "basic/scoped_ptr.h"
#include "storage/storage.h"
#include <mysql.h>

namespace base_storage{
	
class MysqlStorageEngineImpl:public DBStorageEngine{
public:
	MysqlStorageEngineImpl();
	virtual ~MysqlStorageEngineImpl();
public:
	bool Connections(std::list<base::ConnAddr>& addrlist);
	bool Release ();
	bool SQLExec(const char* sql);
	bool Affected(unsigned long& rows);
	bool FreeRes();
	uint32 RecordCount();//由于之前错误，减少代码修改量，此函数返回记录多条
	db_row_t* FetchRows(void);//get Recoder
	bool CheckConnect(void);

	//存储过程
	bool StoredProcedure(){return true;}
	bool AddSPName(const char* sp_name){return true;}
	bool AddSPParam(const int32 var,const int32 type,const char* name,
	        void* param,const int32 outstrlen = 0){return true;}

	 char* GetEntry(db_row_t *row,int cloidx) {return NULL;}

	 char* GetResult(int colidx) {return NULL;}

	 bool CheckAffect(const int32 index,const char* name){return false;}


private:
	scoped_ptr<db_conn_t>    conn_;
	scoped_ptr<db_res_t>     result_;
	scoped_ptr<db_row_t>     rows_;
	bool connected_;
	MYSQL_ROW                row_;
};

}

#endif
