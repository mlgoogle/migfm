#ifndef _MIG_FM_PUBLIC_STORAGE_MSSQL_STRO8AGE_IMPL_H__
#define _MIG_FM_PUBLIC_STORAGE_MSSQL_STROAGE_IMPL_H__
#include <sybdb.h>
#include <list>
#include "basic/basic_info.h"
#include "basic/scoped_ptr.h"
#include "storage/storage.h"

#define ASSERT_CMPCOL(i, n)     assert (strcasecmp (dbcolname ((DBPROCESS*)(conn_.get()->proc), i), n) == 0)

namespace base_storage{
	

class MssqlStorageEngineImpl:public DBStorageEngine{
public:
	MssqlStorageEngineImpl(void);
	virtual ~MssqlStorageEngineImpl(void);
public:
	bool Connections(std::list<base::ConnAddr>& addrlist);
	bool Release ();
	bool SQLExec(const char* sql); //执行语句
	bool AddSPName(const char* sp_name);//添加存储过程名
	bool StoredProcedure(); //执行存储过程
	bool AddSPParam(const int32 var,const int32 type,const char* name,
			        void* param,const int32 outstrlen = 0); //添加参数

	bool Affected(unsigned long& rows);
	bool FreeRes();
	uint32 RecordCount();
	db_row_t* FetchRows(void);//get Recoder
	bool CheckConnect(void);

	char* GetEntry(db_row_t *row,int colidx) ;

	char* GetResult(int colidx);

	bool CheckAffect(const int32 index,const char* name);

private:
	bool AddSPParamBigInt(const int32 var,const char* name,void* param,const int32 outstrlen = 0);
	bool AddSPParamInt(const int32 var,const char* name,void* param,const int32 outstrlen = 0);
	bool AddSPParamStr(const int32 var,const char* name,void* param,const int32 outstrlen = 0);
private:
	//scoped_ptr<db_conn_t>    conn_;
	scoped_ptr<db_conn_t>      conn_;
	scoped_ptr<db_res_t>       result_;
	//scoped_ptr<db_res_t>     result_;
	
	

};
}

#endif
