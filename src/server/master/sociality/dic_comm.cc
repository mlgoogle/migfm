#include "dic_comm.h"
#include "logic_comm.h"
#include "basic/radom_in.h"
#include "json/json.h"
#include <assert.h>
#include <sstream>
#include <hiredis.h>

#if defined (_DIC_POOL_)
#define REDIS_PROC_PROLOG(obj) \
	AutoDicCommEngine auto_engine;\
	base_storage::DictionaryStorageEngine* redis  = auto_engine.GetDicEngine();\
	if (NULL == obj) \
		return false
#endif


namespace mig_sociality {

static const char *HKEY_PUSH_CFG_DEV_TOK = "soc:push.cfg:dev.tok";
static const char *HKEY_PUSH_CFG_IS_RECV = "soc:push.cfg:is.recv";
static const char *HKEY_PUSH_CFG_BTIME = "soc:push.cfg:btime";
static const char *HKEY_PUSH_CFG_ETIME = "soc:push.cfg:etime";
static const char *KEY_PUSH_ID_GEN = "soc:%lld:push.id:next";
static const char *KEY_PUSH_MSG_STAGE = "soc:%lld:push.msg";

static const char *KEY_FRIEND_LIST = "soc:friends:%lld";
static const char *KEY_COMMENT_ID_GEN = "soc:song:%lld:comment.id:next";
static const char *KEY_COMMENT_SSET = "soc:song:%lld:comment";

std::list<base::ConnAddr>  RedisComm::addrlist_;
base::MigRadomIn* RedisComm::radom_num_ = NULL;

#if defined (_DIC_POOL_)
threadrw_t* RedisComm::dic_pool_lock_;
std::list<base_storage::DictionaryStorageEngine*>  RedisComm::dic_conn_pool_;
#endif

AutoDicCommEngine::AutoDicCommEngine()
:engine_(NULL){
#if defined (_DIC_POOL_)
	engine_ = mig_sociality::RedisComm::RedisConnectionPop();
#endif
}

AutoDicCommEngine::~AutoDicCommEngine(){
#if defined (_DIC_POOL_)
	mig_sociality::RedisComm::RedisConnectionPush(engine_);
#endif
}


void RedisComm::Init(std::list<base::ConnAddr>& addrlist,
		const int32 dic_conn_num){
	addrlist_ = addrlist;
	radom_num_ = new base::MigRadomIn();
	//base::SysRadom::InitRandom();
	base::SysRadom::GetInstance();

#if defined (_DIC_POOL_)
	bool r =false;
	InitThreadrw(&dic_pool_lock_);
	for (int i = 0; i<=dic_conn_num;i++){
		base_storage::DictionaryStorageEngine* engine =
				base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);

			if (engine==NULL){
				assert(0);
				continue;
			}
			MIG_DEBUG(USER_LEVEL, "ip:%s,port:%d", addrlist_.front().host().c_str(),
					addrlist_.front().port());
			bool r =  engine->Connections(addrlist_);
			if (!r)
				continue;

		dic_conn_pool_.push_back(engine);
	}

#endif

}

void RedisComm::Dest(){
	if (radom_num_){
		delete radom_num_;
		radom_num_ = NULL;
	}
	//base::SysRadom::DeinitRandom();
	base::SysRadom::FreeInstance();
#if defined (_DIC_POOL_)
	mig_sociality::WLockGd lk(dic_pool_lock_);
	while(dic_conn_pool_.size()>0){
		base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
		dic_conn_pool_.pop_front();
		if(engine){
			engine->Release();
			delete engine;
			engine =NULL;
		}
	}
	DeinitThreadrw(dic_pool_lock_);
#endif

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
		
		if (engine==NULL){
			assert(0);
			return NULL;
		}
		bool r =  engine->Connections(addrlist_);
		if (!r)
			return NULL;
		mig_sociality::ThreadKey::SetStorageDicConn(engine);
		LOG_DEBUG("Created database connection");
		return engine;
	}
	catch (...){
		LOG_ERROR("connect error");
		return NULL;
	}
}

#if defined (_DIC_POOL_)

void RedisComm::RedisConnectionPush(base_storage::DictionaryStorageEngine* engine){
	mig_sociality::WLockGd lk(dic_pool_lock_);
	dic_conn_pool_.push_back(engine);
}

base_storage::DictionaryStorageEngine* RedisComm::RedisConnectionPop(){
	if(dic_conn_pool_.size()<=0)
		return NULL;
	mig_sociality::WLockGd lk(dic_pool_lock_);
    base_storage::DictionaryStorageEngine* engine = dic_conn_pool_.front();
    dic_conn_pool_.pop_front();
    return engine;
}

#endif

bool RedisComm::SetUserPushConfig(int64 uid, const std::string& device_token,
		int is_receive, unsigned begin_time,
		unsigned end_time) {
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis  = auto_engine.GetDicEngine();
#endif
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

bool RedisComm::GetMusicInfos(std::map<std::string,std::string>&temp_songinfo, 
							  std::map<std::string,base::MusicInfo>&songinfo){

#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis  = auto_engine.GetDicEngine();
#endif
	if (NULL == redis)
		  return false;
	redisContext *context = (redisContext *)redis->GetContext();
	std::stringstream os;
	int64 total;
	bool r = false;
	os<<"mget";
	for(std::map<std::string,std::string>::iterator it = temp_songinfo.begin();
		it!=temp_songinfo.end();++it){
			std::string songid;
			Json::Reader reader;
			Json::Value  root;
			r = reader.parse(it->second,root);
			if (!r){
				LOG_ERROR("parser json error");
				continue;
			}
			LOG_DEBUG2("%s",it->second.c_str());
			if (root.isMember("songid")){
				songid = root["songid"].asString();
				os<<" "<<songid.c_str();
			}
	}
	LOG_DEBUG2("%s",os.str().c_str());
	if (NULL == context)
		return false;
	{
		redisReply *rpl = (redisReply *) redisCommand(context,os.str().c_str());
		base_storage::CommandReply *reply = _CreateReply(rpl);
		freeReplyObject(rpl);
		if (NULL == reply)
			return false;

		  //����
		if (base_storage::CommandReply::REPLY_ARRAY == reply->type) {
			base_storage::ArrayReply *arep = 
				static_cast<base_storage::ArrayReply *>(reply);
			base_storage::ArrayReply::value_type &items = arep->value;
			std::map<std::string,std::string>::iterator temp_it = 
				temp_songinfo.begin();
			for (base_storage::ArrayReply::iterator it = items.begin();
				temp_it!= temp_songinfo.end()&&it != items.end();
			    ++it,++temp_it) {
					  base_storage::CommandReply *item = (*it);
					  if (base_storage::CommandReply::REPLY_STRING == item->type) {
						  base_storage::StringReply *srep = 
							  static_cast<base_storage::StringReply *>(item);
						  //songinfolist.push_back(srep->value);
						  base::MusicInfo info;
						  info.UnserializedJson(srep->value);
						  songinfo[temp_it->first] = info;
					  }
			  }
		  }
		  reply->Release();
	  }

	  os.str("");
}

bool RedisComm::IsCollectSong(const std::string& uid,const std::string& songid){
	std::string os;
	bool r = false;
	char* value;
	size_t value_len = 0;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return true;

	os.append("h");
	os.append(uid.c_str());
	os.append("clt");

	r = redis_engine_->GetHashElement(os.c_str(),songid.c_str(),songid.length(),
		&value,&value_len);
	if (r){
		if (value){
			free(value);
			value = NULL;
		}
		return true;

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",songid.c_str());
	}

	return false;
}


bool RedisComm::GetCollectSongs(const std::string& uid, 
								std::map<std::string,std::string>& song_map){
	std::string os;
	bool r = false;
	std::list<std::string> song_list;
	char* value;
	size_t value_len = 0;
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
	if (redis_engine_==NULL)
		return false;

	os.append("h");
	os.append(uid.c_str());
	os.append("clt");
	r = redis_engine_->GetHashValues(os.c_str(),os.length(),song_list);
	//����һ����map�洢 �����ѯ
	while(song_list.size()>0){
		std::string songinfo = song_list.front();
		song_list.pop_front();
		//����json;
		Json::Reader reader;
		Json::Value  root;
		r = reader.parse(songinfo.c_str(),root);
		if (!r)
			continue;
		if (root.isMember("songid")){
			std::string songid = root["songid"].asString();
			song_map[songid] = songinfo;
		}
	}
	return r;
}

bool RedisComm::GetUserPushConfig(int64 uid, std::string& device_token,
		bool& is_receive, unsigned& begin_time, unsigned& end_time) {
	device_token.clear();
	is_receive = true;
	begin_time = 0;
	end_time = 0;

#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis  = auto_engine.GetDicEngine();
#endif
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
#if defined (_DIC_POOL_)
		AutoDicCommEngine auto_engine;
		base_storage::DictionaryStorageEngine* redis_engine_  = auto_engine.GetDicEngine();
#endif
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
	return redis->AddListElement(key, strlen(key), 
		                         msg.c_str(), 
								 msg.length(),1);
}

bool RedisComm::GetStagedPushMsg(int64 uid, int page_index, int page_size, std::list<std::string>& msgs) {
	REDIS_PROC_PROLOG(redis);

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_PUSH_MSG_STAGE, uid);
	int from = page_index * page_size;
	int to = from + page_size;
	return redis->GetListRange(key, strlen(key), from, to, msgs,0);
}

bool RedisComm::AddFriend(int64 uid, int64 touid) {
	REDIS_PROC_PROLOG(redis);

	char key[256] = {0};
	char val[256] = {0};
	snprintf(key, arraysize(key), KEY_FRIEND_LIST, uid);
	size_t key_len = strlen(key);
	snprintf(val, arraysize(val), "%lld", touid);
	size_t val_len = strlen(val);
	return redis->AddListElement(key, strlen(key), val, val_len,1);
}

bool RedisComm::GetFriensList(int64 uid, std::list<std::string>& friends) {
	REDIS_PROC_PROLOG(redis);

	friends.clear();

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_FRIEND_LIST, uid);
	size_t key_len = strlen(key);
	return redis->GetListAll(key, key_len, friends);
}


bool RedisComm::RecordingMsg(const std::string& uid,const base::NormalMsgInfo& msg){
	REDIS_PROC_PROLOG(redis);
	//hash name:uid_msg key:msg id 
	/*std::string name;
	std::stringstream os;
	std::string str;
	name.append(uid);
	name.append("_msg");
	os<<msg.msg_id();
	//msg.SerializedJson(str);
	redis->AddHashElement(name.c_str(),os.str().c_str(),os.str().length(),
		str.c_str(),str.length());*/
}


bool RedisComm::SaveSongComment(int64 songid, int64 uid,
								const std::string &comment,
								const std::string& curjson) {
	REDIS_PROC_PROLOG(redis);

	int64 comment_id = 0;
	std::string os;
	std::stringstream ssongid;
	std::string skey;

	char key[256] = {0};
	char val[256] = {0};
	snprintf(key, arraysize(key), KEY_COMMENT_ID_GEN, songid);
	size_t key_len = strlen(key);

	//������
	//key: a10000t
/*	skey.append("a");
	ssongid<<songid;
	skey.append(ssongid.str().c_str());
	skey.append("t");

	LOG_DEBUG2("key[%s]",skey.c_str());
	LOG_DEBUG2("value[%s]",curjson.c_str());
	redis->SetValue(skey.c_str(),skey.length(),curjson.c_str(),curjson.length());*/

	if (!redis->IncDecValue(key, key_len, 1, comment_id)) {
		return false;
	}

	std::string cur_time;
	mig_sociality::SomeUtils::GetCurrntTimeFormat(cur_time);
	std::stringstream content;
	content	<< "{"
		<< "\"id\":" << comment_id
		<< ",\"uid\":\"" << uid << "\""
		<< ",\"text\":\"" << comment.c_str() << "\""
		<< ",\"createdtime\":\"" << cur_time << "\""
		<< "}"
		<< std::ends;

	std::string content_str = content.str();
	using base_storage::CommandReply;

	char comm_key[256] = {0};
	snprintf(comm_key, arraysize(comm_key), KEY_COMMENT_SSET, songid);

	std::stringstream cmd_ss;
	cmd_ss	<< "ZADD " << comm_key
			<< " " << comment_id
			<< " " << content_str.c_str()
			<< std::ends;
//	CommandReply *reply = redis->DoCommand(
//		"ZADD %s %lld %s", key, comment_id, content_str.c_str());
	redisContext *context = (redisContext *)redis->GetContext();
	if (NULL == context)
		return false;
	redisReply *rpl = (redisReply *)redisCommand(context,
			"ZADD %s %lld %s",
			comm_key, comment_id, content_str.c_str());
	CommandReply *reply = _CreateReply(rpl);
	freeReplyObject(rpl);
	if (NULL == reply)
		return false;
	if (reply->IsError()) {
		reply->Release();
		return false;
	}
	reply->Release();
	return true;
}

bool RedisComm::ReadSongComment(int64 songid, int64 from_id, int count,
		int64 &total, Json::Value& result) {
	REDIS_PROC_PROLOG(redis);

	total = 0;
	result.clear();

	char key[256] = {0};
	snprintf(key, arraysize(key), KEY_COMMENT_SSET, songid);

	using base_storage::CommandReply;
	using base_storage::ArrayReply;
	using base_storage::StringReply;
	using base_storage::IntegerReply;
//	CommandReply *reply = redis->DoCommand(
//			"ZREVRANGEBYSCORE %s +inf (%lld LIMIT 0 %lld", key, from_id, count);
	redisContext *context = (redisContext *)redis->GetContext();
	if (NULL == context)
		return false;

	{
		redisReply *rpl = (redisReply *) redisCommand(context, "ZCARD %s", key);
		CommandReply *reply = _CreateReply(rpl);
		freeReplyObject(rpl);
		if (NULL == reply)
			return false;
		if (CommandReply::REPLY_INTEGER != reply->type) {
			reply->Release();
			return false;
		}
		total = static_cast<IntegerReply *>(reply)->value;
		reply->Release();
	}

	if (0 == total)
		return true;

	{
		redisReply *rpl = (redisReply *) redisCommand(context,
				"ZREVRANGEBYSCORE %s +inf -inf LIMIT %lld %lld", key, from_id,
				count);
		CommandReply *reply = _CreateReply(rpl);
		freeReplyObject(rpl);
		if (NULL == reply)
			return false;
		if (reply->IsError()) {
			reply->Release();
			return false;
		}

		Json::Reader reader;
		if (CommandReply::REPLY_ARRAY == reply->type) {
			ArrayReply *arep = static_cast<ArrayReply *>(reply);
			ArrayReply::value_type &items = arep->value;
			for (ArrayReply::iterator it = items.begin(); it != items.end();
					++it) {
				CommandReply *item = (*it);
				if (CommandReply::REPLY_STRING == item->type) {
					StringReply *srep = static_cast<StringReply *>(item);
					Json::Value jitem;
					if (reader.parse(srep->value, jitem)) {
						result.append(jitem);
					}
				}
			}
		}

		reply->Release();
	}
	return true;
}

base_storage::CommandReply* RedisComm::_CreateReply(redisReply* reply) {
	using namespace base_storage;
	switch (reply->type) {
	case REDIS_REPLY_ERROR:
		return new ErrorReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_NIL:
		return new CommandReply(CommandReply::REPLY_NIL);
	case REDIS_REPLY_STATUS:
		return new StatusReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_INTEGER:
		return new IntegerReply(reply->integer);
	case REDIS_REPLY_STRING:
		return new StringReply(std::string(reply->str, reply->len));
	case REDIS_REPLY_ARRAY: {
		ArrayReply *rep = new ArrayReply();
		for (size_t i = 0; i < reply->elements; ++i) {
			if (CommandReply *cr = _CreateReply(reply->element[i]))
				rep->value.push_back(cr);
		}
		return rep;
	}
	default:
		break;
	}
	return NULL;
}


bool RedisComm::GetMusicAboutUser(const std::string &songid,std::string& content)
{
	char* value;
	size_t value_len = 0;
	std::string key;
	base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	if (redis_engine_==NULL)
		return true;
	key.append("a");
	key.append(songid.c_str());
	key.append("t");
	LOG_DEBUG2("%%%%%key[%s]%%%%%%",key.c_str());
	bool r = redis_engine_->GetValue(key.c_str(),key.length(),
		&value,&value_len);
	if (r){
		content.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}

	}else{
		MIG_ERROR(USER_LEVEL,"GetValue error[%s]",key.c_str());
	}
	return r;
}


void RedisComm::SetMusicAboutUser(const std::string& songid,const std::string& hot_num, 
								  const std::string& cmt_num,const std::string& clt_num){

	  std::string os;
	  std::string key;
	  base_storage::DictionaryStorageEngine* redis_engine_ = GetConnection();
	  if (redis_engine_==NULL)
		  return;
	  //key: a10000t
	  key.append("a");
	  key.append(songid.c_str());
	  key.append("t");

	  //value {"hot":"123","cmt":"231",clt:"2312"}
	  os.append("{\"hot\":\"");
	  os.append(hot_num.c_str());
	  os.append("\",\"cmt\":\"");
	  os.append(cmt_num.c_str());
	  os.append("\",\"clt\":\"");
	  os.append(clt_num.c_str());
	  os.append("\"}");

	  LOG_DEBUG2("key[%s]",key.c_str());
	  LOG_DEBUG2("value[%s]",os.c_str());
	  redis_engine_->SetValue(key.c_str(),key.length(),os.c_str(),os.length());
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


bool MemComm::GetUserCurrentSong(const std::vector<std::string> &vec_user,
								 std::map<std::string, std::string> &map_songs) {
	 using namespace std;

	 map_songs.clear();

	 if (vec_user.empty())
		 return true;

	 typedef std::map<std::string, std::string> KeyUserMap;
	 KeyUserMap map_key_users;
	 std::vector<const char *> vec_keys;
	 std::vector<size_t> vec_key_lens;
	 std::stringstream ss;
	 for (vector<string>::const_iterator it=vec_user.begin(); it!=vec_user.end(); ++it) {
		 ss.str("");
		 ss << "cur" << it->c_str();
		 const string &key = map_key_users.insert(KeyUserMap::value_type(ss.str(), *it)).first->first;
		 LOG_DEBUG2("key [%s]",key.c_str());
		 vec_keys.push_back(key.c_str());
		 vec_key_lens.push_back(key.size());
	 }

	 engine_->MGetValue(&vec_keys[0], &vec_key_lens[0], vec_keys.size());

	 char return_key[MEMCACHED_MAX_KEY] = {0};
	 size_t return_key_length = MEMCACHED_MAX_KEY;
	 char *return_value = NULL;
	 size_t return_value_length = 0;

	 while (engine_->FetchValue(return_key, &return_key_length, &return_value,
		 &return_value_length)) {
			 if (return_key && return_value){
				 map_songs[map_key_users[return_key]] = return_value;
				 MIG_DEBUG(USER_LEVEL,"key[%s] value[%s]",return_key,return_value);
			 }
			 if (return_value)
				 free(return_value);
	 }
	 return true;
}

bool MemComm::SetString(const char* key,const size_t key_len,
						const char* data, size_t len){
	 return engine_->SetValue(key,key_len,data,len);
}

bool MemComm::GetString(const char* key,const size_t key_len,
						char** data,size_t* len){

	 return engine_->GetValue(key,key_len,data,len);
}

bool MemComm::GetCurrentSong(const std::string &key,std::string& value){
	std::string skey;
	skey.append("cur");
	skey.append(key);
	bool r = false;
	size_t svalue_len = 0;
	char* svalue = NULL;
	r = engine_->GetValue(skey.c_str(),skey.length(),&svalue,&svalue_len);
	value.assign(svalue,svalue_len);
	if (r&&svalue!=NULL){
		delete [] svalue;
		svalue = NULL;
	}
	return r;
}

} // mig_sociality
