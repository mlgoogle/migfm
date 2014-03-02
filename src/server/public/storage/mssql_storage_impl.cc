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
	LOG_DEBUG2 ("Msg %d, Level %d, State %d\n", msgno, severity, msgstate);

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
    dbinit();
    dbmsghandle(msg_handler);
    dberrhandle(err_handler);
}

MssqlStorageEngineImpl::~MssqlStorageEngineImpl(){

}

bool MssqlStorageEngineImpl::Connections(std::list<base::ConnAddr>& addrlist){
    
    char* host = NULL;
    char* user = NULL;
    char* pass = NULL;
    char* db_name = NULL;
    db_conn_t* conn = NULL;
    LOGINREC* login;
    DBPROCESS* db_proc;
    
    login = dblogin();
    while(it!=addrlist.end()){
        addr = (*it);
    	host = addr.host();
    	user = addr.usr();
    	pass = addr.pwd();
    	db_name = addr.db();
    	break;
    }
    if(host==NULL||user==NULL||pass==NULL
    	||db_name==NULL){
        MIG_ERROR(USER_LEVEL,"host or user or pass or db  is null");
        return false;
    }
    
    DBSETLUSER(login,user);
    DBSETLPWD(login,pass);
    dbproc = dbopen(login,host);
    if(NULL==dbproc){
        MIG_ERROR(USER_LEVEL,"conncet mssql server error [%s]",host);
        dbloginfree(login);
        return false;
    }
    
    dbcmd(dbproc,"SET ANSI_WARNINGS ON");
    dbsqlexec(dbproc);
    
    dbcmd(dbproc,"SET ANSI_PADDING ON");
    dbsqlexec(dbproc);
    
    dbcmd(dbproc,"SET CONCAT_NULL_YIELDS_NULL ON");
    dbsqlexec(dbproc);
    
    dbuse(dbproc,(char*)dbname);
    dblogin_free(login);
    
    conn_ = (db_conn_t*)malloc(sizeof(db_conn_t));
    
    if(NULL==conn){
        MIG_ERROR(USER_LEVEL,"malloc error");
        return false;
    }
    
    conn->proc = dbproc;
    return conn;
}


bool MssqlStorageEngineImpl::Release(){
	if(conn){
	    dbclose(conn->proc);
	    free(conn);
	}
	return true;
}

}