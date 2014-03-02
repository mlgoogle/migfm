#ifndef _MIG_FM_PUBLIC_STORAGE_MSSQL_STROAGE_IMPL_H__
#define _MIG_FM_PUBLIC_STORAGE_MSSQL_STROAGE_IMPL_H__
#include <sybdb.h>
#include "basic/basic_info.h"

namespace base_storage{
	
class MssqlStorageEngineImpl:DBStorageEngine{
public:
	MssqlStorageEngineImpl(void);
	virtual ~MssqlStorageEngineImpl(void);
public:
    bool Connections(std::list<base::ConnAddr>& addrlist);
	bool Release ();
   // bool SQLExec(const char* sql) = 0;
	//bool Affected(unsigned long& rows) = 0;
   // bool FreeRes() = 0;
	//db_row_t* FetchRows(void) = 0;//get Recordset
	//bool CheckConnect(void) =0;
private:
	db_conn_t*    conn_;
	
	
};
}

#endif