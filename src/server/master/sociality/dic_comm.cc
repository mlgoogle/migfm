#include "dic_comm.h"
#include "logic_comm.h"
#include <assert.h>

#define REDIS_PROC_PROLOG(obj) \
	base_storage::DictionaryStorageEngine *obj = GetConnection(); \
	if (NULL == obj) \
		return false

namespace storage{

static const char *HKEY_PUSH_CFG_DEV_TOK = "soc:push.cfg:dev.tok";
static const char *HKEY_PUSH_CFG_IS_RECV = "soc:push.cfg:is.recv";
static const char *HKEY_PUSH_CFG_BTIME = "soc:push.cfg:btime";
static const char *HKEY_PUSH_CFG_ETIME = "soc:push.cfg:etime";
static const char *KEY_PUSH_ID_GEN = "soc:%lld:push.id:next";
static const char *KEY_PUSH_MSG_STAGE = "soc:%lld:push.msg";

static const char *KEY_FRIEND_LIST = "soc:friends:%lld";

std::list<base::ConnAddr>  RedisComm::addrlist_;
base::MigRadomIn* RedisComm::radom_num_ = NULL;
void RedisComm::Init(std::list<base::ConnAddr>& addrlist){
	addrlist_ = addrlist;
	radom_num_ = new base::MigRadomIn();
	base::SysRadom::InitRandom();
}

void RedisComm::Dest(){
	if (radom_num_){
		delete radom_num_;
		radom_num_ = NULL;
	}
	base::SysRadom::DeinitRandom();
}

base_storage::DictionaryStorageEngine* RedisComm::GetConnection(){

	try{
		base_storage::DictionaryStorageEngine* engine =
			mig_sociality::ThreadKey::GetStorageDicConn();
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
		engine->Connections(addrlist_);
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		mig_sociality::ThreadKey::SetStorageDicConn(engine);
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

bool RedisComm::GetMusicInfos(const std::string& songid, std::string &music_infos) {
	char* value = NULL;
	size_t value_len = 0;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;
	bool r = redis_engine_->GetValue(songid.c_str(),songid.length(),
		&value,&value_len);
	if (r && value){
		music_infos.assign(value,value_len-1);
		free(value);
		value = NULL;
	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",songid.c_str());
	}

	return r;
}

bool RedisComm::GenaratePushMsgID(int64 uid, int64& msg_id) {
	REDIS_PROC_PROLOG(redis);

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_PUSH_ID_GEN, uid);
	return redis->IncDecValue(key, strlen(key), 1, msg_id);
}

bool RedisComm::StagePushMsg(int64 uid, int64 msg_id, const std::string& msg) {
	REDIS_PROC_PROLOG(redis);

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_PUSH_MSG_STAGE, uid);
	return redis->AddListElement(key, strlen(key), msg.c_str(), msg.length());
}

bool RedisComm::GetStagedPushMsg(int64 uid, int page_index, int page_size, std::list<std::string>& msgs) {
	REDIS_PROC_PROLOG(redis);

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_PUSH_MSG_STAGE, uid);
	int from = page_index * page_size;
	int to = from + page_size;
	return redis->GetListRange(key, strlen(key), from, to, msgs);
}

bool RedisComm::AddFriend(int64 uid, int64 touid) {
	REDIS_PROC_PROLOG(redis);

	char key[256] = {0};
	char val[256] = {0};
	snprintf(key, arraysize(key), KEY_FRIEND_LIST, uid);
	size_t key_len = strlen(key);
	snprintf(val, arraysize(val), "%lld", touid);
	size_t val_len = strlen(val);
	return redis->AddListElement(key, strlen(key), val, val_len);
}

bool RedisComm::GetFriensList(int64 uid, std::list<std::string>& friends) {
	REDIS_PROC_PROLOG(redis);

	friends.clear();

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_FRIEND_LIST, uid);
	size_t key_len = strlen(key);
	return redis->GetListAll(key, key_len, friends);
}

/////////////////////////////////memcahced//////////////////////////////////////
base_storage::DictionaryStorageEngine* MemComm::engine_ = NULL;

void MemComm::Init(std::list<base::ConnAddr>& addrlist){
	engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_MEM);
	engine_->Connections(addrlist);
}

void MemComm::Dest(){
	if (engine_){
		delete engine_;
		engine_ = NULL;
	}
}

}
