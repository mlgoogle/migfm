#include "robot_song_mgr.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
namespace robot_logic{

RobotSongMgr::RobotSongMgr(){

}

RobotSongMgr::~RobotSongMgr(){

}

bool RobotSongMgr::OnUserDefaultSong(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg, int len){

	struct NoticeUserDefaultSong* vNoticeUserDefaultSong = (struct NoticeUserDefaultSong*)packet;
	robot_base::RobotBasicInfo robotinfo;
	//查找对应的机器人
	bool r = CacheManagerOp::GetRobotCacheMgr()->GetUserFolowRobot(vNoticeUserDefaultSong->platform_id,
			vNoticeUserDefaultSong->uid,1,robotinfo);
	if(!r)
		return false;
	//测试，用户听什么歌曲就赠送什么歌曲
	struct NoticeUserRobotHandselSong notice_handsel_song;
	MAKE_HEAD(notice_handsel_song, NOTICE_USER_ROBOT_HANDSEL_SONG,USER_TYPE,0,0);
	notice_handsel_song.platform_id = vNoticeUserDefaultSong->platform_id;
	notice_handsel_song.uid = vNoticeUserDefaultSong->uid;
	notice_handsel_song.robot_id = robotinfo.uid();
	notice_handsel_song.song_id = vNoticeUserDefaultSong->songid;
	return sendmessage(robotinfo.socket(),&notice_handsel_song);
}
}
