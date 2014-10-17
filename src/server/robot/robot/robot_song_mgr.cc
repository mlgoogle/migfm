#include "robot_song_mgr.h"
#include "robot_cache_manager.h"
#include "robot_basic_info.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_unit.h"
#include "base/logic_comm.h"
#include "base/comm_head.h"
#include "basic/base64.h"
#include <sstream>
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
	sendrobotmssage(robotinfo,&notice_handsel_song);
	return r;
}


bool RobotSongMgr::OnNoticeUserChangerSong(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg/* = NULL*/, int len/* = 0*/){
	struct NoticeUserCurrentSong* current_song = (struct NoticeUserCurrentSong*)packet;
	bool r = false;
	std::stringstream os;
	std::list<int64> list;
	std::list<std::string> songinfolist;
	std::map<std::string,base::MusicInfo>  musicinfomap;
	RobotInfosMap robot_infos;
	//获取用户对应的机器人
	r = CacheManagerOp::GetRobotCacheMgr()->GetUserFollowAllRobot(current_song->platform_id,
			current_song->uid,robot_infos);
	if(!r)
		return false;

	r = CacheManagerOp::GetRobotCacheMgr()->GetModeRadomSong(current_song->platform_id,
			current_song->mode,current_song->type_id,robot_infos.size(),list); //获取随机ID
	if(!r)
		return false;
	os<<current_song->mode<<"_r"<<current_song->type_id;
	//获取音乐信息
	r = robot_storage::RedisComm::GetBatchMusicInfos(os.str(),list,songinfolist);
	if(!r)
		return false;
	//存储用户音乐信息
	CacheManagerOp::GetRobotCacheMgr()->SetUserListenState(current_song->platform_id,current_song->uid,
			current_song->type_id,current_song->mode);

	//转化音乐信息
	robot_logic::LogicUnit::FormateMusicInfo(songinfolist,musicinfomap);
	RobotInfosMap::iterator it = robot_infos.begin();
	std::map<std::string,base::MusicInfo>::iterator itr = musicinfomap.begin();
	//遍历发送机器人
	for(;it!=robot_infos.end()&&itr!=musicinfomap.end();it++,itr++){
		base::MusicInfo musicinfo = itr->second;
		robot_base::RobotBasicInfo robot = it->second;
		robot.set_follower_user_last_time(time(NULL));
		struct NoticeUserListenSong notice_user_listen;
		MAKE_HEAD(notice_user_listen, NOTICE_USER_ROBOT_LISTEN_SONG,USER_TYPE,0,0);
		notice_user_listen.platform_id = current_song->platform_id;
		notice_user_listen.songid = atoll(musicinfo.id().c_str());
		notice_user_listen.typid = current_song->type_id;

		std::string b64title;
		std::string b64artist;
		Base64Decode(musicinfo.title(),&b64title);
		Base64Decode(musicinfo.artist(),&b64artist);
		memset(&notice_user_listen.mode,'\0',MODE_LEN);
		snprintf(notice_user_listen.mode, arraysize(notice_user_listen.mode),
				current_song->mode.c_str());
		memset(&notice_user_listen.singer,'\0',SINGER_LEN);
		snprintf(notice_user_listen.singer, arraysize(notice_user_listen.singer),
					"%s",b64artist.c_str());
		memset(&notice_user_listen.name,'\0',NAME_LEN);
		snprintf(notice_user_listen.name, arraysize(notice_user_listen.name),
						"%s",b64title.c_str());
		sendrobotmssage(robot,&notice_user_listen);

	}
	return r;
}

bool RobotSongMgr::OnRobotLoginSong(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg/* = NULL*/, int len/* = 0*/){

	struct RobotLogin* vRobotLogin  = (struct RobotLogin*)packet;
	bool r = false;
	int64 songid;
	std::string musicinfo;
	Json::Value value;
	std::string mode = "chl";
	r = robot_storage::DBComm::GetRobotLoginListenSong(vRobotLogin->uid,songid);
	if(!r)
		return false;

	r = robot_storage::RedisComm::GetMusicInfos(songid,musicinfo);
	if(!r)
		return false;
	//刚刚登陆成功 故不需要修改时间
	SendRobotListenSong(vRobotLogin->platform_id,songid,1,mode,socket);

/*
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
*/
	return true;
}

bool RobotSongMgr::ResolveJsonMusic(const std::string& musicinfo,Json::Value& value){
	   Json::Reader reader;
	   bool r = reader.parse(musicinfo.c_str(),value);
	   return r;
}


bool RobotSongMgr::SendRobotListenSong(const int64 platform_id,const int64 songid,const int64 type_id,
		const std::string& mode,int socket){

	bool r = false;
	std::string musicinfo;
	Json::Value value;

	r = robot_storage::RedisComm::GetMusicInfos(songid,musicinfo);
	if(!r)
		return false;
	r = ResolveJsonMusic(musicinfo,value);
		//发送歌曲给机器人
	struct NoticeUserListenSong notice_user_listen;
	MAKE_HEAD(notice_user_listen, NOTICE_USER_ROBOT_LISTEN_SONG,USER_TYPE,0,0);
	notice_user_listen.platform_id = platform_id;
	notice_user_listen.songid = songid;
	notice_user_listen.typid = type_id;

	memset(&notice_user_listen.mode,'\0',MODE_LEN);
	snprintf(notice_user_listen.mode, arraysize(notice_user_listen.mode),
			mode.c_str());

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

}
