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
	bool OnRobotLogin(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);//机器人登陆

	bool OnRobotChatLogin(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);//通知登陆聊天服务器//用户断开连接把机器人进行回收

	bool OnClearRobotConnection(const int socket);//清理连接
};
}

#endif
