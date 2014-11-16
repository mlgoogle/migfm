#include "mssql_storage_impl.h"
#include "log/mig_log.h"
namespace base_storage{

static int err_handler (DBPROCESS *dbproc, int severity, int dberr, int oserr, char *dberrstr, char *oserrstr)
{
    if (dberrstr != NULL)
        MIG_ERROR(USER_LEVEL,"DB-LIBRARY error:\n\t%s\n", dberrstr);

    if (oserr != DBNOERR)
        MIG_ERROR(USER_LEVEL,"Operating-system error:\n\t%s\n", oserrstr);

    if (dbproc == NULL || DBDEAD (dbproc))
        return INT_EXIT;

    return INT_CANCEL;
}

static int msg_handler (DBPROCESS *dbproc, DBINT msgno, int msgstate, int severity, char *msgtext, char *srvname, char *procname, int line)
{
#if _DEBUG
	MIG_DEBUG (USER_LEVEL,"Msg %d, Level %d, State %d\n", msgno, severity, msgstate);

	if (srvname != NULL)
		MIG_DEBUG(USER_LEVEL,"Server '%s', ", srvname);
	if (procname != NULL)
		MIG_DEBUG(USER_LEVEL,"Procedure '%s', ", procname);
	if (line !=  0)
		MIG_DEBUG(USER_LEVEL,"Line %d", line);

	MIG_DEBUG(USER_LEVEL,"\n\t%s\n", msgtext);
#endif
	return 0;
}

MssqlStorageEngineImpl::MssqlStorageEngineImpl(){
    //result_.reset(new db_res_t);
    //rows_.reset(new db_row_t);
   // conn_.get()->proc = result_.get()->proc = rows_.get()->proc = NULL;

    dbinit();
    dbmsghandle(msg_handler);
    dberrhandle(err_handler);
}

MssqlStorageEngineImpl::~MssqlStorageEngineImpl(){
	dbexit();
}

bool MssqlStorageEngineImpl::Connections(std::list<base::ConnAddr>& addrlist){
    
    const char* host = NULL;
    const char* user = NULL;
    const char* pass = NULL;
    const char* db_name = NULL;
    //db_conn_t* conn = NULL;
    base::ConnAddr addr;
    LOGINREC* login;
    DBPROCESS* dbproc;
    
    std::list<base::ConnAddr>::iterator it = addrlist.begin();
    while(it!=addrlist.end()){
        addr = (*it);
    	host = addr.host().c_str();
    	user = addr.usr().c_str();
    	pass = addr.pwd().c_str();
    	db_name = addr.source().c_str();
    	break;
    }
    if(host==NULL||user==NULL||pass==NULL
    	||db_name==NULL){
        MIG_ERROR(USER_LEVEL,"host or user or pass or db  is null");
        return false;
    }
    
    login = dblogin();

    if (login==NULL){
    	MIG_ERROR(USER_LEVEL,"login null [%s]",strerror(errno));
   		return false;
   	}

    DBSETLUSER (login, user);
    DBSETLPWD (login, pass);
       //DBSETLPACKET(login, 512);
    dbproc = dbopen (login, (char *) host);
    if (dbproc == FAIL) {
    	dbloginfree (login);
    	return false;
    }

    dbcmd (dbproc, "SET ANSI_WARNINGS ON");
    dbsqlexec (dbproc);
    dbcmd (dbproc, "SET ANSI_PADDING ON");
    dbsqlexec (dbproc);
    dbcmd (dbproc, "SET CONCAT_NULL_YIELDS_NULL ON");
    dbsqlexec (dbproc);

    dbuse (dbproc, (char *)db_name);
    dbloginfree (login);

   /* conn_.reset(new db_conn_t);
    if (conn_ == NULL)
    	return false;

    conn_.get()->proc = dbproc;
*/
    //conn_ = (db_conn_t *) malloc (sizeof (db_conn_t));
    //if (conn_ == NULL)
      // return false;
    conn_.reset(new db_conn_t );
    conn_.get()->proc = dbproc;


    return true;
}


//添加名称
bool MssqlStorageEngineImpl::AddSPName(const char* sp_name){
	/*dbcancel ((DBPROCESS*)(conn_->proc));
	if(dbrpcinit((DBPROCESS*)(conn_->proc), sp_name,(DBSMALLINT) 0) == FAIL){
		assert(0);
		return false;
	}*/


	dbcancel ((DBPROCESS*)(conn_.get()->proc));
	if (dbrpcinit ((DBPROCESS*)(conn_.get()->proc), sp_name, (DBSMALLINT) 0) == FAIL)
		return false;

	return true;
}


bool MssqlStorageEngineImpl::Release(){
/*	if(conn_.get()){
		DBPROCESS* proc = (DBPROCESS*)(conn_.get()->proc);
	    dbclose(proc);
	    free(conn_.get());
	}
	*/
	return true;
}


bool MssqlStorageEngineImpl::Affected(unsigned long& rows){

	return true;
}

uint32 MssqlStorageEngineImpl::RecordCount(){
	/*DBPROCESS* proc = (DBPROCESS*)(result_.get()->proc);*/
	return dbnumrets((DBPROCESS*)(result_.get()->proc));
	//return true;
}

bool MssqlStorageEngineImpl::FreeRes(){
	/*DBPROCESS* connporc =(DBPROCESS*)(result_.get()->proc);
	DBPROCESS* resultporc =(DBPROCESS*)(result_.get()->proc);
	dbcancel (connporc);
    dbcanquery (resultporc);
    free (result_.get());
    */
	return true;
}

bool MssqlStorageEngineImpl::CheckAffect(const int32 index,const char* name){
	MIG_INFO(USER_LEVEL,"name %s name %s",dbcolname ((DBPROCESS*)(conn_.get()->proc), index),name);
	MIG_INFO(USER_LEVEL,"result %d", strcasecmp (dbcolname ((DBPROCESS*)(conn_.get()->proc), index),name));
	//ASSERT_CMPCOL(index,name);
	assert((strcasecmp (dbcolname ((DBPROCESS*)(conn_.get()->proc), index),name)==0));
	return true;
}

db_row_t* MssqlStorageEngineImpl::FetchRows(void){
    RETCODE rc;
    db_row_t *row;

    row = (db_row_t *) malloc (sizeof (db_row_t));
    if (row == NULL)
        return NULL;
    row->proc = result_->proc;

    rc = dbnextrow ((DBPROCESS*)(result_.get()->proc));

	switch (rc){
	case SUCCEED:
		return row;

	case REG_ROW:
		return row;

	case NO_MORE_ROWS:
	case FAIL:
	case BUF_FULL:
		free(row);
		return NULL;

	default:
		free(row);
		return NULL;
	}
	return row;
}

bool MssqlStorageEngineImpl::CheckConnect(void){
	return true;
}
//执行语句
bool MssqlStorageEngineImpl::SQLExec(const char* sql){

	/*DBPROCESS* proc = (DBPROCESS*)(conn_.get()->proc);
	dbcmd(proc,sql);*/

	return true;
}

//添加参数
bool MssqlStorageEngineImpl::AddSPParam(const int32 var,const int32 type,const char* name,
        void* param,const int32 outstrlen){

	bool r = false;
	/*switch(type){
		case base_storage::TYPE_INT64:
			r = AddSPParamBigInt(var,name,param);
			break;
		case base_storage::TYPE_INT32:
			r = AddSPParamInt(var,name,param);
			break;
		case base_storage::TYPE_CHAR:
			r = AddSPParamStr(var,name,param,outstrlen);
			break;
		default:
			r = false;
	}*/

	 //if (dbrpcparam ((DBPROCESS*)(conn_->proc), "@sysid", (BYTE) 0, SYBINT4, -1, -1, (BYTE *)param) == FAIL)
		// return false;

	return true;
}

char* MssqlStorageEngineImpl::GetResult(int colidx){
    char *buf;
    int l = dbretlen ((DBPROCESS *)(result_.get()->proc), colidx + 1);

    if (l <= 0)
        return NULL;

    buf = (char *) malloc (l + 1);
    if (buf == NULL)
        return NULL;

    memcpy (buf, (DBCHAR *) dbretdata ((DBPROCESS *)(result_.get()->proc), colidx + 1), l);
    buf[l] = '\0';
    return buf;
}

char* MssqlStorageEngineImpl::GetEntry(db_row_t *row,int colidx){
    DBPROCESS *proc = (DBPROCESS *)(row->proc);
    BYTE *ptr = dbdata (proc, colidx + 1);
    int t = dbcoltype (proc, colidx + 1);
    int l = dbcollen (proc, colidx + 1);
    char *buf;
    int buflen = 2 * l + 1;
    int datlen = dbdatlen (proc, colidx + 1);

    buf = (char *) malloc (buflen);
    if (buf == NULL)
        return NULL;

    if (dbwillconvert (t, SYBCHAR)) {
        DBINT r = dbconvert (proc, t, ptr, datlen, SYBCHAR, (BYTE *) buf, -1);
        assert (r !=  -1);
        buf[r] = '\0';
        //LOG_DEBUG2 ("db_get_entry1: %s[%d, %d, %d]", buf, l, datlen, r);
        return buf;
    }
    memcpy (buf, ptr, datlen);
    buf[datlen] = '\0';
    //LOG_DEBUG2 ("db_get_entry2: %s[%d, %d]", buf, l, datlen);
    return buf;
}

//执行存储过程
bool MssqlStorageEngineImpl::StoredProcedure(){

	if (dbrpcsend ((DBPROCESS*)(conn_.get()->proc)) == FAIL) {
		 assert (0);
		 return NULL;
	 }

	 if (dbsqlok ((DBPROCESS*)(conn_.get()->proc)) == FAIL) {
		 assert (0);
		 return NULL;
	 }

	 if (dbresults ((DBPROCESS*)(conn_.get()->proc)) != SUCCEED) {
		 assert (0);
		 return NULL;
	 }

	 //result_ = (db_res_t *) malloc (sizeof (db_res_t));
	 result_.reset(new db_res_t);
	 if (result_.get() == NULL)
		 return NULL;
	 result_.get()->proc = conn_.get()->proc;
	 return true;
}


//添加整型参数
bool MssqlStorageEngineImpl::AddSPParamBigInt(const int32 var,const char* name,void* param,const int32 outstrlen){
	DBPROCESS* proc = (DBPROCESS*)(conn_.get()->proc);
	/*switch(var){
		case PARAM_IN:
		    if (dbrpcparam (proc, name, (BYTE) 0, SYBINT8, -1, -1, (BYTE *)param) == FAIL)
		        return false;
		    break;
		case PARAM_OUT:
		    if (dbrpcparam (proc, name, (BYTE) DBRPCRETURN, SYBINT8, -1, 0, (BYTE *)NULL) == FAIL)
		        return false;
		    break;
		default:
			return false;
	}*/
	return true;
}

bool MssqlStorageEngineImpl::AddSPParamInt(const int32 var,const char* name,void* param,const int32 outstrlen){
	DBPROCESS* proc = (DBPROCESS*)(conn_.get()->proc);
	/*switch(var){
		case PARAM_IN:
			if (dbrpcparam (proc, name, (BYTE) 0, SYBINT4, -1, -1, (BYTE *)param) == FAIL)
				return false;
			break;
		case PARAM_OUT:
		    if (dbrpcparam (proc, name,(BYTE) DBRPCRETURN, SYBINT4, -1, 0, (BYTE *)NULL) == FAIL)
		    	return false;
		    break;
		default:
			return false;
	}*/
	return true;
}

//添加字符串参数
bool MssqlStorageEngineImpl::AddSPParamStr(const int32 var,const char* name,void* param,const int32 outstrlen){
	DBPROCESS* proc = (DBPROCESS*)(conn_.get()->proc);
	/*switch(var){
		case PARAM_IN:
		    if (dbrpcparam (proc, name, (BYTE) 0, SYBCHAR, 0, strlen ((char*)param), (BYTE *)param) == FAIL)
		        return false;
		    break;
		case PARAM_OUT:
		    if (dbrpcparam (proc, name, (BYTE) DBRPCRETURN, SYBCHAR, outstrlen, 0, (BYTE *)NULL) == FAIL)
		        return false;
		    break;
	}*/
	return true;
}

}
