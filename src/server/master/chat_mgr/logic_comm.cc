#include "logic_comm.h"
#include "dic_comm.h"
#include "basic/basic_util.h"
#include <sys/socket.h>
namespace chat_logic{

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

bool SomeUtils::CheckUserToken(const std::string& platform_id,
		                      const std::string& uid,const std::string& token){
	std::string new_token;
	bool r = GetUserToken(platform_id,uid,new_token);
	if(r){
		if(new_token == token)
			return true;
	}

	return false;
}

bool SomeUtils::GetUserToken(const std::string& platform_id,
		                    const std::string& uid,std::string& token){
     return chat_storage::MemComm::GetUserToken(platform_id,uid,token);
}

int SomeUtils::SendFull(int socket, const char *buffer, size_t nbytes){
	ssize_t amt = 0;
	ssize_t total = 0;
	const char *buf = buffer;

	do {
		amt = nbytes;
		LOG_DEBUG2("(%s)",buf);
		amt = send (socket, buf, amt, 0);
		buf = buf + amt;
		nbytes -= amt;
		total += amt;
	} while (amt != -1 && nbytes > 0);

	return (int)(amt == -1 ? amt : total);
}

void SomeUtils::GetUTF8(std::string &msg,std::string &out_str,
							 int32 flag/* = 1*/){
	char* out;
	size_t out_len;
	std::stringstream os;
	if (flag){
		base::BasicUtil::GB2312ToUTF8(msg.c_str(),msg.length(),
			&out,&out_len);
		out_str.assign(out,out_len);
		if(out){
			delete [] out;
			out = NULL;
		}
	}else{
		LOG_DEBUG("===========================================");
		out_str = os.str();
	}

	LOG_DEBUG2("%s",out_str.c_str());

}

}
