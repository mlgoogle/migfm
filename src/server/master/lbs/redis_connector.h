/*
 * redis_connector.h
 *
 *  Created on: 2013-7-7
 *      Author: huaiyu
 */

#ifndef _REDIS_CONNECTOR_H_
#define _REDIS_CONNECTOR_H_

#include "basic/basictypes.h"

namespace base_storage {
	class DictionaryStorageEngine;
}

namespace mig_lbs {

class RedisConnector {
public:
	static RedisConnector *GetInstance();
	static void FreeInstance();

public:
	bool Connect();
	bool IsConnected() const { return NULL != redis_; }

	bool BindUserPOI(int64 user_id, int64 poi_id);
	int64 FindUserPOIID(int64 user_id);
	bool DeleteUserPOI(int64 user_id);

public:
	~RedisConnector();

private:
	RedisConnector();

	base_storage::DictionaryStorageEngine *redis_;

private:
	static RedisConnector *instance_;

	DISALLOW_COPY_AND_ASSIGN(RedisConnector);
};

} /* namespace mig_lbs */

#endif /* _REDIS_CONNECTOR_H_ */
