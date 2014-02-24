#include "logic_comm.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
#include "log/mig_log.h"
#include <sstream>
#include <sys/socket.h>
#include <time.h>

namespace mig_sociality {
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
		if (tv->conn_){
			delete tv->conn_;
			tv->conn_ = NULL;
		}
		free (tv);
		ThreadkeySet (NULL, ThreadKey::db_key_);

		LOG_DEBUG ("Close Dic connection");
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

void SomeUtils::GetResultMsg(std::string &status, std::string &msg, 
							 std::string &result,std::string &out_str,
							 int32 flag/* = 1*/){
	char* out;
	size_t out_len;
	std::stringstream os;
	os<<"{\"status\":"<<status.c_str()<<",\"msg\":\""
		<<msg.c_str()<<"\",\"result\":{"<<result.c_str()
		<<"}}";
	if (flag){
		base::BasicUtil::GB2312ToUTF8(os.str().c_str(),os.str().length(),
			&out,&out_len);
		out_str.assign(out,out_len);
	}else{
		out_str = os.str();
	}

	LOG_DEBUG2("%s",out_str.c_str());

}

void SomeUtils::GetCurrntTimeFormat(std::string& current_time){
	time_t current = time(NULL);
	struct tm* local = localtime(&current);
	std::stringstream os;
	os<<(1900+local->tm_year)<<"-"
	  <<(1+local->tm_mon)<<"-"
	  <<local->tm_mday<<" "
	  <<local->tm_hour<<":"
	  <<local->tm_min<<":"
	  <<local->tm_sec;
	current_time = os.str();
}

int SomeUtils::SplitStringChr( const char *str, const char *char_set,
				   std::vector<std::string> &out )
{
	using std::string;

	assert(str != NULL);
	assert(char_set != NULL);

	out.clear();

	if (0 == char_set[0]) {
		if (str[0])
			out.push_back(str);
		return (int)out.size();
	}

	const char *find_ptr = NULL;
	str += ::strspn(str, char_set);
	while (str && (find_ptr=::strpbrk(str, char_set))) {
		if (str != find_ptr)
			out.push_back(string(str, find_ptr));
		str = find_ptr + ::strspn(find_ptr, char_set);
	}
	if (str && str[0])
		out.push_back(str);

	return (int)out.size();
}

} // mig_sociality
