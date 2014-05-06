#ifndef _MIG_FM_PUBLIC_STORAGE_MSSQL_STRO8AGE_IMPL_H__
#define _MIG_FM_PUBLIC_STORAGE_MSSQL_STROAGE_IMPL_H__
#include <sybdb.h>
#include "basic/basic_info.h"
#include "basic/scoped_ptr.h"
#include "storage/storage.h"

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
			        void* param); //添加参数

	bool Affected(unsigned long& rows);
	bool FreeRes();
	uint32 RecordCount();
	db_row_t* FetchRows(void);//get Recoder
	bool CheckConnect(void);

	char* GetEntry(db_row_t *row,int colidx) ;
private:
	bool AddSPParamBigInt(const int32 var,const char* name,void* param);
	bool AddSPParamInt(const int32 var,const char* name,void* param);
	bool AddSPParamStr(const int32 var,const char* name,void* param);
private:
	//scoped_ptr<db_conn_t>    conn_;
	scoped_ptr<db_conn_t>      conn_;
	scoped_ptr<db_res_t>       result_;
	//scoped_ptr<db_res_t>     result_;
	
	

};
}

#endif
