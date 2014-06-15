#ifndef __MIG_ROBOT_SCHEDULER_MGR_H__
#define __MIG_ROBOT_SCHEDULER_MGR_H__
#include "robot_basic_info.h"
#include "logic_unit.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include "common.h"

namespace robot_logic{

class SchedulerMgr{
public:
	SchedulerMgr();
	virtual ~SchedulerMgr();
public:
	bool OnSchedulerMgrLogin(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);

	bool OnDisQuiteScheduler(const int socket);
};
}
#endif
