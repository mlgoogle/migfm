#ifndef _MIG_FM_PUSH_DB_COMM_H__
#define _MIG_FM_PUSH_DB_COMM_H__
#include "basic_push_info.h"
#include "basic/basic_info.h"
#include "basic/basictypes.h"
#include <list>

namespace base_push{

class PushDBComm{
public:
	PushDBComm();
	virtual ~PushDBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool BindBDPushUserinfo(const int64 platform,const int64 uid,const int64 channel,
			const std::string& baidu_userid,const std::string& pkg_name,const std::string& tag,
			const std::string& appid,const std::string& requestid,const int32 machine);

	static bool BindBDPushUserinfo(const int64 platform,const int64 uid,const int64 channel,
			const int64 baidu_userid,const std::string& pkg_name,const std::string& tag,
			const int64 appid,const int64 requestid,const int32 machine);

	static bool GetUserBdInfo(const int64 platform,const int64 uid,
			base_push::BaiduPushMessage& message);

	static bool GetAllPushMessage(std::list<base_push::BaiduPushMessage*>& list);

	static bool UpdateAllPushMessage(const int64 msg_id);
};


}

#endif
