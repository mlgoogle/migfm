#include "robot_connection.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
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
	double latitude = 0;
	double longitude = 0;
	struct NoticeUserLogin* vNoticeUserLogin = (struct NoticeUserLogin*)packet;
	std::list<robot_base::RobotBasicInfo> list;
	//获取改用户的坐标
	robot_storage::DBComm::GetUserLbsPos(vNoticeUserLogin->uid,latitude,longitude);
	r = CacheManagerOp::GetRobotCacheMgr()->GetIdleRobot(vNoticeUserLogin->platform_id,vNoticeUserLogin->uid,
			latitude,longitude,list);
	if(list.size()<=0)
		return false;

	//存储用户信息
	CacheManagerOp::GetRobotCacheMgr()->SetUserInfoLogin(vNoticeUserLogin->platform_id,vNoticeUserLogin->uid);

	//通知机器人调度客户端，机器人登录
	struct NoticeRobotLogin notice_robot_login;
	MAKE_HEAD(notice_robot_login, NOTICE_USER_ROBOT_LOGIN,USER_TYPE,0,0);
	notice_robot_login.uid = vNoticeUserLogin->uid;
	while(list.size()>0){
		robot_base::RobotBasicInfo robot_basic_info = list.front();
		list.pop_front();
		struct RobotInfo* robot_info = new struct RobotInfo;
		robot_info->uid = robot_basic_info.uid();
		robot_info->songid = 0;
		robot_info->latitude = robot_basic_info.latitude();
		robot_info->longitude = robot_basic_info.longitude();
		memset(&robot_info->nickname,'\0',NICKNAME_LEN);
		robot_logic::SomeUtils::SafeStrncpy(robot_info->nickname,NICKNAME_LEN,
					robot_basic_info.nickname().c_str(),NICKNAME_LEN);
		notice_robot_login.robot_list.push_back(robot_info);
	}

	robot_logic::CacheManagerOp::GetRobotCacheMgr()->SchedulerSendMessage(vNoticeUserLogin->platform_id,&notice_robot_login);
	return true;
}

bool RobotConnection::OnRobotLogin(struct server *srv, int socket, struct PacketHead *packet,
        const void *msg, int len){
	bool r = false;
	robot_base::RobotBasicInfo robot_info;
	struct RobotLogin* vRobotLogin= (struct RobotLogin*)packet;
	r = CacheManagerOp::GetRobotCacheMgr()->RobotLoginSucess(vRobotLogin->platform_id,
							vRobotLogin->robot_id,socket,vRobotLogin->uid,robot_info);
	//获取机器人信息
	//保存socket
	r = CacheManagerOp::GetCacheManagerOp()->SetRobotInfo(socket,robot_info);
	return r;
}

bool RobotConnection::OnRobotChatLogin(struct server *srv, int socket, struct PacketHead *packet,
        const void *msg, int len){
	bool r = false;
	int flag = 0;
	robot_base::RobotBasicInfo robotinfo;
	struct NoticeUserRobotChatLogin* vNoticeUserRobotChatLogin = (struct NoticeUserRobotChatLogin*)packet;
	//需要检测是否已经在和对应用户聊天
	//检测改机器人是否已经使用
	r = CacheManagerOp::GetRobotCacheMgr()->GetUsedRobotInfo(vNoticeUserRobotChatLogin->platform_id,
			vNoticeUserRobotChatLogin->robot_id,robotinfo);

	if(!r){
		r = CacheManagerOp::GetRobotCacheMgr()->GetIdelRobotInfo(vNoticeUserRobotChatLogin->platform_id,
				vNoticeUserRobotChatLogin->robot_id,robotinfo);
		if(!r)
			return false;
		flag = 0;
	}else{
		flag = 1;
	}

	//不管是否全部通知调用服务端登陆聊天服务端
	struct NoticeUserRobotChatLogin notice_robot_chat_login;
	MAKE_HEAD(notice_robot_chat_login, NOTICE_USER_ROBOT_SCHEDULER_CHAT_LOGIN,USER_TYPE,0,0);
	notice_robot_chat_login.platform_id = vNoticeUserRobotChatLogin->platform_id;
	notice_robot_chat_login.uid = vNoticeUserRobotChatLogin->uid;
	notice_robot_chat_login.robot_id = vNoticeUserRobotChatLogin->robot_id;
	robot_logic::CacheManagerOp::GetRobotCacheMgr()->SchedulerSendMessage(vNoticeUserRobotChatLogin->platform_id,&notice_robot_chat_login);
	return true;
}

bool RobotConnection::OnClearRobotConnection(const int socket){
	//
	bool r = false;
	robot_base::RobotBasicInfo robot_info;
	r = CacheManagerOp::GetCacheManagerOp()->GetRobotInfo(socket,robot_info);
	if(!r)
		return r;
	r = CacheManagerOp::GetRobotCacheMgr()->ClearRobot(10000,robot_info);
	CacheManagerOp::GetCacheManagerOp()->DelRobotInfo(socket);

	return true;
}


}
