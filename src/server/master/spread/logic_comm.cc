#include "logic_comm.h"
#include "dic_comm.h"
#include "http_response.h"
#include "basic/basic_util.h"
#include "json/json.h"
#include <sys/socket.h>
namespace spread_logic{

static const std::string URL_PUSH_SERVICE("121.199.32.88/api/push");

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

void SomeUtils::GetCurrentTimeFormat(const time_t current,std::string& current_time)
{
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


bool HttpComm::PushMessage(const std::string &device_token,
		const std::string &msg, int badge/*=1*/, const std::string &sound/*=""*/) {

	//return true;
	if (device_token.empty())
		return false;

	spread_logic::HttpPost post(URL_PUSH_SERVICE);
	Json::Value value;
	Json::FastWriter wr;
	std::string post_str;
	value["message"] = msg;
	value["badge"] = badge;
	value["devicetoken"] = device_token;
	value["sound"] = sound;
	post_str = wr.write(value);

	LOG_DEBUG2("Push msg post:%s", post_str.c_str());
	int port = 9090;
	try {
		post.Post(post_str.c_str(),port);
		std::string result;
		post.GetContent(result);
		LOG_DEBUG2("%s",result.c_str());

		Json::Reader rd;
		Json::Value value;
		if (!rd.parse(result, value)) {
			LOG_DEBUG("Push msg failed: ����ʧ��");
			return false;
		}

	} catch (const std::exception &ex) {
		LOG_DEBUG2("Push msg failed:%s", ex.what());
		return false;
	}catch (...) {
		LOG_DEBUG("Push msg failed");
		return false;
	}
	return true;
}


//WeiXin
bool HttpComm::WeiXin::GetWeiXinToken(const std::string& appid,const std::string& secret,
								std::string& content){
	std::stringstream os;
	std::string url;
	bool r = false;
	os<<"https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid="
			<<appid<<"&secret="<<secret;
	url = os.str();
	spread_logic::HttpResponse respone(url);
	r = respone.Get();
	if(!r)
		return r;
	r = respone.GetContent(content);
	if(!r)
		return r;
	return r;
}

bool HttpComm::WeiXin::PostWeiXinMenu(const std::string& access_token,const std::string& content,
						std::string& result){
	std::stringstream os;
	std::string url;
	bool r =false;
	os<<"https://api.weixin.qq.com/cgi-bin/menu/create?access_token="<<access_token;
	url = os.str();
	spread_logic::HttpPost http_post(url);
	r = http_post.Post(content.c_str(),443);
	if(!r)
		return r;
	http_post.GetContent(result);
	if(!r)
		return r;
	return r;
}


//WeiXin

bool ResolveJson::WeiXin::ResolveWeiXinGetToken(const std::string& content,std::string& token,
	      int32 exp_time){
	 Json::Reader reader;
	 Json::Value  root;
	 bool r = false;
	 r = reader.parse(content.c_str(),root);
	 if (!r){
		 LOG_ERROR2("parse json error[%s]",content.c_str());
		 return false;
	 }

	 token = root["access_token"].asString();
	 exp_time = root["expires_in"].asInt();
	 return true;

}

bool ResolveJson::WeiXin::ResolveWeiXinGetMenuState(const std::string& content){
	 Json::Reader reader;
	 Json::Value  root;
	 int32 errcode;
	 std::string errmsg;
	 bool r = false;
	 r = reader.parse(content.c_str(),root);
	 if (!r){
		 LOG_ERROR2("parse json error[%s]",content.c_str());
		 return false;
	 }
	 errmsg = root["errmsg"].asString();
	 errcode = root["errcode"].asInt();
	 if(errcode==0)
		 return true;
	 return false;

}

}
