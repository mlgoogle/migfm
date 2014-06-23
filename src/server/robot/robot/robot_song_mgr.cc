#include "robot_song_mgr.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
#include "basic/base64.h"
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
	bool r = CacheManagerOp::GetRobotCacheMgr()->GetUserFollowTaskRobot(vNoticeUserDefaultSong->platform_id,
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


bool RobotSongMgr::OnRobotLoginSong(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg/* = NULL*/, int len/* = 0*/){

	struct RobotLogin* vRobotLogin  = (struct RobotLogin*)packet;
	bool r = false;
	int64 songid;
	std::string musicinfo;
	Json::Value value;
	r = robot_storage::DBComm::GetRobotLoginListenSong(vRobotLogin->uid,songid);
	if(!r)
		return false;

	r = robot_storage::RedisComm::GetMusicInfos(songid,musicinfo);
	if(!r)
		return false;
	r = ResolveJsonMusic(musicinfo,value);
		//发送歌曲给机器人
	struct NoticeUserListenSong notice_user_listen;
	MAKE_HEAD(notice_user_listen, NOTICE_USER_ROBOT_LISTEN_SONG,USER_TYPE,0,0);
	notice_user_listen.platform_id = vRobotLogin->platform_id;
	notice_user_listen.songid = songid;
	notice_user_listen.typid = 1;

	memset(&notice_user_listen.mode,'\0',MODE_LEN);
	snprintf(notice_user_listen.mode, arraysize(notice_user_listen.mode),
		"chl");

	//logic::SomeUtils::SafeStrncpy(notice_user_listen.mode,MODE_LEN,
		//		"chl",MODE_LEN);
	//notice_user_listen.mode = "chl";
	std::string title = value["titile"].asString();
	std::string artist = value["artist"].asString();
	//base64转码
	std::string b64title;
	std::string b64artist;
	Base64Decode(title,&b64title);
	Base64Decode(artist,&b64artist);
	LOG_DEBUG2("title %s b64title %s",title.c_str(),b64title.c_str());
	LOG_DEBUG2("artist %s b64artist %s",artist.c_str(),b64artist.c_str());
	memset(&notice_user_listen.singer,'\0',SINGER_LEN);
	snprintf(notice_user_listen.singer, arraysize(notice_user_listen.singer),
			"%s",b64artist.c_str());
	memset(&notice_user_listen.name,'\0',NAME_LEN);
	snprintf(notice_user_listen.name, arraysize(notice_user_listen.name),
				"%s",b64title.c_str());
	sendmessage(socket,&notice_user_listen);
	return true;
}

bool RobotSongMgr::ResolveJsonMusic(const std::string& musicinfo,Json::Value& value){
	   Json::Reader reader;
	   bool r = reader.parse(musicinfo.c_str(),value);
	   return r;
}

}
