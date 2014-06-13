#include "robot_interface.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "base/logic_comm.h"
#include "common.h"
#include "plugins.h"


__attribute__((visibility("default")))
int NoticeUserLogin(int socket,	int64 platform_id,int64 uid,
					double latitude,double longitude){
	struct NoticeUserLogin notice_user_login;
	MAKE_HEAD(notice_user_login, NOTICE_USER_LOGIN,USER_TYPE,0,0);
	notice_user_login.uid = uid;
	notice_user_login.latitude = latitude;
	notice_user_login.longitude = longitude;
	//logic::SomeUtils::SendMessage(socket,&notice_user_login,__FILE__,__LINE__);
	sendmessage(socket,&notice_user_login);
}
