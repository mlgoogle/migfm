#ifndef _ROBOT_ROBOT_INTERFACE_H__
#define _ROBOT_ROBOT_INTERFACE_H__
#include "basic/basictypes.h"

#ifdef __cplusplus 
extern "C" {
#endif

//int robot_manager_plugin_init(struct plugin *pl);
int NoticeUserLogin(int socket,	int64 platform_id,int64 uid,
					double latitude,double longitude);//通知用户登录接口

int NoticeUserDefaultSong(const int socket,const int64 platform_id,const int64 uid,
			const int64 songid,const int32 type_id,const char* mode);

int NoticeUserCurrentSong(const int socket,const int64 platform_id,const int64 uid,
			const int64 songid,const int32 type_id,const char* mode);

int NoticeRobotLogin(const int socket,const int64 platform_id,const int64 uid,
					const int64 robotid);//通知机器人登陆聊天服务器

int NoticeUserGiftLuck(const int socket,const int64 platform_id,const int64 uid,
			const int64 songid,const int plat);//通知用户完成分享开始抽奖
#ifdef __cplusplus 
}
#endif


#endif
