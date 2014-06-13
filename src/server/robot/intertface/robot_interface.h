#ifndef _ROBOT_ROBOT_INTERFACE_H__
#define _ROBOT_ROBOT_INTERFACE_H__
#include "basic/basictypes.h"

#ifdef __cplusplus 
extern "C" {
#endif

//int robot_manager_plugin_init(struct plugin *pl);
int NoticeUserLogin(int socket,	int64 platform_id,int64 uid,
					double latitude,double longitude);//通知用户登录接口

#ifdef __cplusplus 
}
#endif


#endif
