#include "robot_connection.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
namespace robot_logic{

RobotConnection::RobotConnection(){

}

RobotConnection::~RobotConnection(){

}

bool RobotConnection::OnUserLogin(struct server *srv, int socket, struct PacketHead *packet,
        const void *msg, int len){
	//读取空闲机器人数据
	bool r = false;
	struct NoticeUserLogin* vNoticeUserLogin = (struct NoticeUserLogin*)packet;
	std::list<robot_base::RobotBasicInfo> list;
	r = CacheManagerOp::GetRobotCacheMgr()->GetIdleRobot(vNoticeUserLogin->platform_id,vNoticeUserLogin->uid,list);
	if(list.size()<=0)
		return false;

	//通知机器人调度客户端，机器人登录
	struct NoticeRobotLogin notice_robot_login;
	notice_robot_login.uid = vNoticeUserLogin->uid;
	while(list.size()>0){
		robot_base::RobotBasicInfo robot_basic_info = list.front();
		list.pop_front();
		struct RobotInfo* robot_info = new struct RobotInfo;
		robot_info->uid = robot_basic_info.uid();
		robot_info->songid = 0;
		robot_info->latitude = robot_basic_info.latitude();
		robot_info->longitude = robot_basic_info.longitude();
		logic::SomeUtils::SafeStrncpy(robot_info->nickname,NICKNAME_LEN,
					robot_basic_info.nickname().c_str(),NICKNAME_LEN);
		notice_robot_login.robot_list.push_back(robot_info);
	}

	robot_logic::CacheManagerOp::GetRobotCacheMgr()->SchedulerSendMessage(vNoticeUserLogin->platform_id,&notice_robot_login);
	return true;
}

}
