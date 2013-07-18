/*
 * redis_connector.cc
 *
 *  Created on: 2013-7-7
 *      Author: huaiyu
 */

#include "redis_connector.h"
#include "storage/storage.h"
#include "log/mig_log.h"

namespace mig_lbs {

bool RedisConnector::BindUserPOI(int64 user_id, int64 poi_id) {
	ASSERT(IsConnected());

	if (0==user_id || 0==poi_id)
		return false;

	char key[256] = {0};
	char val[256] = {0};
	snprintf(key, arraysize(key), "user:%lld:pos.id", user_id);
	snprintf(val, arraysize(val), "%lld", poi_id);

	return redis_->AddValue(key, strlen(key), val, strlen(val));
}

int64 RedisConnector::FindUserPOIID(int64 user_id) {
	ASSERT(IsConnected());

	if (0 == user_id)
		return 0;

	char key[256] = {0};
	char *val = NULL; //[1024] = {0};
	snprintf(key, arraysize(key), "user:%lld:pos.id", user_id);
	size_t val_len = 0;
	redis_->GetValue(key, strlen(key), &val, &val_len);
	if (NULL==val || 0==val_len)
		return 0;
	else
		return atoll(val);
}

bool RedisConnector::DeleteUserPOI(int64 user_id) {
	ASSERT(IsConnected());

	if (0 == user_id)
		return false;

	char key[256] = {0};
	snprintf(key, arraysize(key), "user:%lld:pos.id", user_id);

	return redis_->DelValue(key, strlen(key));
}


RedisConnector::RedisConnector()
	: redis_(NULL) {
	// TODO Auto-generated constructor stub
}

bool RedisConnector::Connect() {
	using namespace base;
	using namespace base_storage;

	if (NULL != redis_)
		return true;

	redis_ = base_storage::DictionaryStorageEngine::Create(IMPL_RADIES);
	if (NULL == redis_)
		return false;

	std::list<base::ConnAddr> addrlist;
	addrlist.push_back(ConnAddr("localhost", 6379));
	if (!redis_->Connections(addrlist)) {
		redis_->Release();
		redis_ = NULL;
		MIG_WARN(USER_LEVEL, "redis connects failed!");
		return false;
	}

	MIG_DEBUG(USER_LEVEL, "redis connected:%s:%d", "localhost", 6379);
	return true;
}

RedisConnector::~RedisConnector() {
	// TODO Auto-generated destructor stub
	if (redis_)
		redis_->Release();
}

} /* namespace mig_lbs */
