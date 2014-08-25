#ifndef _MIG_FM_LBS_DB_COMM_H__
#define _MIG_FM_LBS_DB_COMM_H__
#include "basic_lbs_info.h"
#include "basic/basic_info.h"
#include <list>

namespace base_lbs{

class LBSDBComm{
public:
	LBSDBComm();
	virtual ~LBSDBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetBaiduAcessKey(const int64 platform_id,std::list<base_lbs::BaiDuAccessKey>& list);
};

}

#endif
