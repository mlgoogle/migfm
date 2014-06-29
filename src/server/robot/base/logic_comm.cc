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


char *SomeUtils::SafeStrncpy (char *dest, size_t dest_size, const char *src, size_t src_size)
{
	size_t n = dest_size < src_size ? dest_size : src_size;
	memset(dest,'\0',dest_size);
	memcpy (dest, src, n);
	return dest;
}


int SomeUtils::SendFull(int socket, const char *buffer,
						size_t nbytes){
	ssize_t amt = 0;
	ssize_t total = 0;
	const char *buf = buffer;

	do {
		amt = nbytes;
		amt = send (socket, buf, amt, 0);
		buf = buf + amt;
		nbytes -= amt;
		total += amt;
	} while (amt != -1 && nbytes > 0);

	return (int)(amt == -1 ? amt : total);
}

bool SomeUtils::SendBytes(int socket, const void *bytes, int len,
						  const char *file, long line){
	if (socket <= 0 || bytes == NULL || len <= 0)
	  return false;

	int ret = SendFull (socket, (char *) bytes, len);
	if (ret != len) {
	  LOG_ERROR2 ("Sent bytes failed in %s:%d", file, line);
	  return false;
	}

	LOG_DEBUG ("Sent bytes success");
	return true;
}

bool SomeUtils::SendMessage(int socket, struct PacketHead *packet,
							 const char *file, long line){

	bool r;
	void *packet_stream = NULL;
	int32_t packet_stream_length = 0;
	int ret = 0;
	bool  r1 = false;
	if (socket <= 0 || packet == NULL)
		return false;

	if (ProtocolPack::PackStream (packet, &packet_stream, &packet_stream_length) == false) {
		LOG_ERROR2 ("Call PackStream failed in %s:%d", file, line);
		r = false;
		goto MEMFREE;
	}

	//LOG_DEBUG2("opcode[%d]\n",packet->operate_code);
	//ProtocolPack::DumpPacket(packet);
	ret = SendFull (socket, (char *) packet_stream, packet_stream_length);
	ProtocolPack::HexEncode(packet_stream,packet_stream_length);
	if (ret != packet_stream_length) {
		LOG_ERROR2 ("Sent msg failed in %s:%d", file, line);
		r = false;
		goto MEMFREE;
	} else {
		r = true;
		goto MEMFREE;
	}
	MEMFREE:
	char* stream = (char*)packet_stream;
	if (stream){
		delete[] stream;
		stream = NULL;
	}

	return r;
}

bool SomeUtils::SendHeadMessage(int socket, int32 operate_code, int16 msg_type,
								int8 is_zip,int session,
								const char *file, long line){
	struct PacketHead packet;
	//MAKE_FAIL_HEAD (packet, operate_code,session);
	int64 msg_id = base::SysRadom::GetInstance()->GetRandomID();
	//MAKE_FAIL_HEAD(head, _operate_code,_msg_type,_is_zip,_reserverd)
	MAKE_FAIL_HEAD(packet,operate_code,msg_type,is_zip,session);
	return SendMessage (socket, &packet, file, line);
}

void SomeUtils::CloseSocket(int socket){
	close(socket);
}

bool SomeUtils::SendErrorCode(int socket,int32 operate_code,int16 msg_type,
		                      int8 is_zip,int32 session,const int32 error_code,
							  const char *file,long line){
	/*const char* error_msg = migchat_strerror(error_code);
	struct ChatFailed chat_failed;
	MAKE_HEAD(chat_failed,operate_code,msg_type,is_zip,session);
	//logic::SomeUtils::SafeStrncpy(chat_failed.error_msg.c_str(),chat_failed.error_msg.length(),
		//						error_msg,strlen(error_msg));
	chat_failed.platform_id = 10000;
	chat_failed.error_msg.assign(error_msg);
	return SendMessage(socket,&chat_failed,file,line);
	*/
	return true;
}




}
