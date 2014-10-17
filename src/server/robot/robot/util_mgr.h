#ifndef _ROBOT_ROBOT_UTIL_MGR_H__
#define _ROBOT_ROBOT_UTIL_MGR_H__
#include "logic_unit.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include "json/json.h"
#include "common.h"
namespace robot_logic{

enum LUCK_PRIZE{
	FIRST_PRIZE = 1,
	SECOND_PRIZE = 2,
	THIRD_PRIZE = 3
};

class UtilMgr{
public:
	UtilMgr();
	virtual ~UtilMgr();
public:
	bool OnUserReadyLuckGift(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);
public:
	bool LuckGiftCalculation(const int32 plat,const int64 uid,const int64 songid);
};
}
#endif
