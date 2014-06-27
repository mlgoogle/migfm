#include "robot_interface.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "base/logic_comm.h"
#include "common.h"
#include "plugins.h"


__attribute__((visibility("default")))
int NoticeUserLogin(int socket,	int64 platform_id,int64 uid,
					double latitude,double longitude){
	if(socket<=0)
		return 0;
	struct NoticeUserLogin notice_user_login;
	MAKE_HEAD(notice_user_login, NOTICE_USER_LOGIN,USER_TYPE,0,0);
	notice_user_login.platform_id = platform_id;
	notice_user_login.uid = uid;
	notice_user_login.latitude = latitude;
	notice_user_login.longitude = longitude;
	//logic::SomeUtils::SendMessage(socket,&notice_user_login,__FILE__,__LINE__);
	sendmessage(socket,&notice_user_login);
}

int NoticeUserDefaultSong(const int socket,const int64 platform_id,const int64 uid,
			const int64 songid,const int32 type_id,const char* mode){
	if(socket<=0)
		return 0;
	struct NoticeUserDefaultSong notice_user_default_song;
	MAKE_HEAD(notice_user_default_song,NOTICE_USER_DEFAULT_SONG,USER_TYPE,0,0);
	notice_user_default_song.platform_id = platform_id;
	notice_user_default_song.uid = uid;
	notice_user_default_song.songid = songid;
	notice_user_default_song.type_id = type_id;
	notice_user_default_song.mode = mode;
	sendmessage(socket,&notice_user_default_song);
}

int NoticeUserCurrentSong(const int socket,const int64 platform_id,const int64 uid,
			const int64 songid,const int32 type_id,const char* mode){
	if(socket<=0)
		return 0;
	struct NoticeUserCurrentSong notice_user_current_song;
	MAKE_HEAD(notice_user_current_song,NOTICE_USER_CURRENT_SONG,USER_TYPE,0,0);
	notice_user_current_song.platform_id = platform_id;
	notice_user_current_song.uid = uid;
	notice_user_current_song.songid = songid;
	notice_user_current_song.type_id = type_id;
	notice_user_current_song.mode = mode;
	sendmessage(socket,&notice_user_current_song);
}
