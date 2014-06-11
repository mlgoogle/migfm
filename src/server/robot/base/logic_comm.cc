#include "logic_comm.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
#include "log/mig_log.h"
#include "basic/radom_in.h"
#include <sstream>
#include <sys/socket.h>
#include <time.h>

namespace logic{
	
typedef struct {
	base_storage::DBStorageEngine *conn_;
} ThreadValueDB;

typedef struct{
	base_storage::DictionaryStorageEngine *conn_;
}ThreadValueDic;


threadkey_handler_t *ThreadKey::db_key_ = NULL;
threadkey_handler_t *ThreadKey::dic_key_ = NULL;

static void keydestdb (void *v)
{
	ThreadValueDB *tv = (ThreadValueDB *)v;
	if (tv) {
		if (tv->conn_)
			tv->conn_->Release();
		if (tv->conn_){
			delete tv->conn_;
			tv->conn_ = NULL;
		}
		free (tv);
		ThreadkeySet (NULL, ThreadKey::db_key_);

		LOG_DEBUG ("Close database connection");
	}
}

static void keydestdic(void *v)
{
	ThreadValueDic *tv = (ThreadValueDic *)v;
	if (tv) {
		if (tv->conn_)
			tv->conn_->Release();
		free (tv);
		ThreadkeySet (NULL, ThreadKey::db_key_);

		LOG_DEBUG ("Close dic connection");
	}
}

void ThreadKey::InitThreadKey(){
	int r = ThreadkeyInit(&db_key_,keydestdb);
	r = ThreadkeyInit(&dic_key_,keydestdic);
}

base_storage::DBStorageEngine* ThreadKey::GetStorageDBConn(){
	ThreadValueDB* v = (ThreadValueDB*)ThreadkeyGet(db_key_);
	return v?v->conn_:NULL;
}

void ThreadKey::SetStorageDBConn(base_storage::DBStorageEngine *conn){
	ThreadValueDB* v = (ThreadValueDB*)ThreadkeyGet(db_key_);
	if (v==NULL)
		v = (ThreadValueDB *)calloc(1,sizeof(ThreadValueDB));
	v->conn_ = conn;
	ThreadkeySet(v,db_key_);
}

base_storage::DictionaryStorageEngine* ThreadKey::GetStorageDicConn(){
	ThreadValueDic* v = (ThreadValueDic*)ThreadkeyGet(dic_key_);
	return v?v->conn_:NULL;
}

void ThreadKey::SetStorageDicConn(base_storage::DictionaryStorageEngine *conn){
	ThreadValueDic* v = (ThreadValueDic*)ThreadkeyGet(dic_key_);
	if (v==NULL)
		v = (ThreadValueDic *)calloc(1,sizeof(ThreadValueDic));
	v->conn_ = conn;
	ThreadkeySet(v,dic_key_);
}

void ThreadKey::DeinitThreadKey(){
	ThreadkeyDeinit(db_key_);
	ThreadkeyDeinit(dic_key_);
}




}
