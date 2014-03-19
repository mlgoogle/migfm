#include "dic_comm.h"
#include "base/logic_comm.h"
#include <sstream>

namespace chat_storage{

static const char *HKEY_PUSH_CFG_DEV_TOK = "soc:push.cfg:dev.tok";
static const char *HKEY_PUSH_CFG_IS_RECV = "soc:push.cfg:is.recv";
static const char *HKEY_PUSH_CFG_BTIME = "soc:push.cfg:btime";
static const char *HKEY_PUSH_CFG_ETIME = "soc:push.cfg:etime";

base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;
std::list<base::ConnAddr>  RedisComm::addrlist_;

bool MemComm::Init(std::list<base::ConnAddr>& addrlist){
	bool r = false;
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	if (engine_==NULL)
		return false;
	r = engine_->Connections(addrlist);
	return r;
}

void MemComm::Dest(){
	if (engine_){
		delete engine_;
		engine_ = NULL;
	}
}

bool MemComm::GetUserInfo(const int64 platform_id,int64 user_id, 
						  chat_base::UserInfo& userinfo){
   //key uidinfo 10000info
	std::stringstream os;
	std::string key;
	bool r = false;
	char* mem_value = NULL;
	size_t mem_value_length = 0;
	os<<user_id<<"info";
	key = os.str();
	r = engine_->GetValue(key.c_str(),key.length(),&mem_value,&mem_value_length);
	if (!r)
		return false;
	r = userinfo.UnserializedJson(mem_value);
	if (mem_value){
		delete [] mem_value;
		mem_value = NULL;
	}
	if (!r)
		return r;

}

bool MemComm::CheckToken(const int64 platform_id,int64 user_id, 
						 const char* current_token){
	 //key:platform(10001)_userid(30003): 10001_30003
	std::stringstream os;
	bool r = false;
	os<<platform_id<<"_"<<user_id;
	std::string key;
	size_t value_len = 0;
	char* value = NULL;
	key = os.str();
	r = engine_->GetValue(key.c_str(),key.length(),&value,&value_len);
	if (!r)
		return false;
	if (strcmp(value,current_token)==0)
		return true;
	return false;
}


void RedisComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
}

void RedisComm::Dest(){
}

base_storage::DictionaryStorageEngine* RedisComm::GetConnection(){

	try{
		base_storage::DictionaryStorageEngine* engine =
				logic::ThreadKey::GetStorageDicConn();
		if (engine){
// 			if (!engine->){
// 				LOG_ERROR("Database %s connection was broken");
// 				engine->Release();
// 				if (engine){
// 					delete engine;
// 					engine = NULL;
// 				}
// 			}else
				return engine;
		}

		engine =
			base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);

		if (engine==NULL){
			assert(0);
			return NULL;
		}
		bool r =  engine->Connections(addrlist_);
		if (!r)
			return NULL;
		logic::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

bool RedisComm::SetUserPushConfig(int64 uid, const std::string& device_token,
		int is_receive, unsigned begin_time,
		unsigned end_time) {
	base_storage::DictionaryStorageEngine *redis = GetConnection();
	if (NULL == redis)
		return false;

	char field[256] = {0};
	char val[256] = {0};
	snprintf(field, arraysize(field), "%lld", uid);
	size_t field_len = strlen(field);

	redis->SetHashElement(HKEY_PUSH_CFG_DEV_TOK, field, field_len,
			device_token.c_str(), device_token.length());

	snprintf(val, arraysize(val), "%d", is_receive);
	redis->SetHashElement(HKEY_PUSH_CFG_IS_RECV, field, field_len,
			val, strlen(val));

	snprintf(val, arraysize(val), "%u", begin_time);
	redis->SetHashElement(HKEY_PUSH_CFG_BTIME, field, field_len,
			val, strlen(val));

	snprintf(val, arraysize(val), "%d", end_time);
	redis->SetHashElement(HKEY_PUSH_CFG_ETIME, field, field_len,
			val, strlen(val));

	return true;
}

bool RedisComm::GetUserPushConfig(int64 uid, std::string& device_token,
		bool& is_receive, unsigned& begin_time, unsigned& end_time) {
	device_token.clear();
	is_receive = true;
	begin_time = 0;
	end_time = 0;

	base_storage::DictionaryStorageEngine *redis = GetConnection();
	if (NULL == redis)
		return false;

	char field[256] = {0};
	char *val = NULL; //[1024] = {0};
	snprintf(field, arraysize(field), "%lld", uid);
	size_t field_len = strlen(field);
	size_t val_len = 0;
	bool ret = false;

	ret = redis->GetHashElement(HKEY_PUSH_CFG_DEV_TOK, field, field_len, &val, &val_len);
	if (ret && val) {
		device_token.assign(val, val_len);
		free(val);
	} else {
		return false;
	}

	val = NULL;
	val_len = 0;
	ret = redis->GetHashElement(HKEY_PUSH_CFG_IS_RECV, field, field_len, &val, &val_len);
	if (ret && val) {
		is_receive = atoi(val)!=0 ? true : false;
		free(val);
	} else {
		is_receive = true;
	}

	val = NULL;
	val_len = 0;
	redis->GetHashElement(HKEY_PUSH_CFG_BTIME, field, field_len, &val, &val_len);
	if (ret && val) {
		begin_time = atoi(val);
		free(val);
	} else {
		begin_time = DEFAULT_BEGIN_TIME;
	}

	val = NULL;
	val_len = 0;
	redis->GetHashElement(HKEY_PUSH_CFG_ETIME, field, field_len, &val, &val_len);
	if (ret && val) {
		end_time = atoi(val);
		free(val);
	} else {
		end_time = DEFAULT_END_TIME;
	}

	return true;
}




}
