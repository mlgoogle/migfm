#ifndef _ROBOT_ROBOT_CONNECTION_H__
#define _ROBOT_ROBOT_CONNECTION_H__

#include "robot_basic_info.h"
#include "logic_unit.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include "common.h"

namespace robot_logic{

class RobotConnection{
public:
	RobotConnection();
	virtual ~RobotConnection();
public:
	bool OnUserLogin(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);//真实用户登录
};
}

#endif
