#include "sociality_mgr_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include "basic/base64.h"
#include "basic/errno_comm.h"
#include "json/json.h"
#include "storage/db_storage.h"
#include "storage/dic_storage.h"
#include "http_comm.h"
#include <sstream>
#include <ctime>

#define		TIME_TEST		1025

#define		LOGIC_PROLOG()	\
		status = 0;	\
		err_code = 0

namespace mig_sociality{


SocialityMgrEngine::SocialityMgrEngine(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	mig_sociality::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	DBComm::Init(config->mysql_db_list_);
	MemComm::Init(config->mem_list_);
	RedisComm::Init(config->redis_list_);
	base_storage::MYSQLDB::Init(config->mysql_db_list_);
	base_storage::MemDic::Init(config->mem_list_);
}

SocialityMgrEngine::~SocialityMgrEngine(){
	DBComm::Dest();
	MemComm::Dest();
	RedisComm::Dest();

	mig_sociality::ThreadKey::DeinitThreadKey();
}

SocialityMgrEngine* SocialityMgrEngine::instance_ = NULL;
SocialityMgrEngine* SocialityMgrEngine::GetInstance(){
   if (instance_==NULL){
        instance_ = new SocialityMgrEngine();
   }

   return instance_;
}

void SocialityMgrEngine::FreeInstance(){
	delete instance_;
}

bool SocialityMgrEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool SocialityMgrEngine::OnBroadcastConnect(struct server *srv, int socket, 
									  void *data, int len){
    return true;
}

bool SocialityMgrEngine::OnBroadcastMessage(struct server *srv, int socket, 
										void *msg, int len){
    return true;
}

bool SocialityMgrEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
    return true;
}

bool SocialityMgrEngine::OnTimeout(struct server *srv, char *id, int opcode, int time){

	return true;
}

bool SocialityMgrEngine::OnClose(struct server *srv, int socket){
    return true;
}

bool SocialityMgrEngine::OnConnect(struct server *srv, int socket){
    return true;
}

bool SocialityMgrEngine::OnReadMessage(struct server *srv, int socket,
								       const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);

	Json::Value root(Json::objectValue);
	int ret_status = 0;
	int err_code = 0;
	if (type == "setuserconfigofpush") {
		OnMsgSetUserConfigOfPush(packet, root, ret_status, err_code);
	} else if (type == "presentsong") {
		OnMsgPresentSong(packet, root, ret_status, err_code);
	} else if (type == "getpushmsg") {
		OnMsgGetPushMsg(packet, root, ret_status, err_code);
	} else if (type == "getpushmsgsummary") {
		OnMsgGetPushMsgSummary(packet, root, ret_status, err_code);
	} else if (type == "getfriendlist") {
		OnMsgGetFriendList(packet, root, ret_status, err_code);
	} else if (type == "sendfriendmsg") {
		OnMsgSendFriendMsg(packet, root, ret_status, err_code);
	} else if (type == "sayhello") {
		OnMsgSayHello(packet, root, ret_status, err_code);
	} else if (type == "adduserbacklist") {
		OnMsgAddUserBacklist(packet, root, ret_status, err_code);
	} else if (type == "addfriend") {
		OnMsgAddFriend(packet, root, ret_status, err_code);
	} else if (type == "importsonglist") {
		OnMsgImportSongList(packet, root, ret_status, err_code);
	} else if (type == "importfriend") {
		OnMsgImportFriend(packet, root, ret_status, err_code);
	} else if (type == "commentsong") {
		OnMsgCommentSong(packet, root, ret_status, err_code);
	} else if (type == "get_comment") {
		OnMsgGetComment(packet, root, ret_status, err_code);
	} else if (type == "getmusicuser"){
		OnMsgGetMusicFriend(packet,root,ret_status, err_code);
	}else {
		return true;
	}

	root["status"] = ret_status;
	if (ret_status != 1)
		root["msg"] = migfm_strerror(err_code);
	else
		root["msg"] = "";

	Json::FastWriter wr;
	std::string res = wr.write(root);
	SomeUtils::SendFull(socket, res.c_str(), res.length());

	MIG_DEBUG(USER_LEVEL, "lbs request:%s, response:%s", type.c_str(), res.c_str());

    return true;
}

bool SocialityMgrEngine::OnMsgSetUserConfigOfPush(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();

	std::string user_id, device_token, is_receive, begin_time, end_time;
	if (!packet.GetAttrib("uid", user_id)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("devicetoken", device_token)) {
		err_code = MIG_FM_HTTP_DEVICE_TOKEN_NOT_EXIST;
		return false;
	}
	if (!packet.GetAttrib("isreceive", is_receive)) {
		is_receive = "1";
	}
	if (!packet.GetAttrib("begintime", begin_time)) {
		begin_time = "08:00";
	}
	if (!packet.GetAttrib("endtime", end_time)) {
		end_time = "22:00";
	}

	int64 uid = atoll(user_id.c_str());
	if (0 == uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

	unsigned btime = 0, etime = 0;
	if (!CheckAndTransHMTime(begin_time, btime)) {
		err_code = MIG_FM_HTTP_INVALID_TIME_FORMAT;
		return false;
	}
	if (!CheckAndTransHMTime(end_time, etime)) {
		err_code = MIG_FM_HTTP_INVALID_TIME_FORMAT;
		return false;
	}

	if (!RedisComm::SetUserPushConfig(uid, device_token,
		atoi(is_receive.c_str()), btime, etime)) {
		err_code = MIG_FM_DB_SAVE_PUSH_CONFIG_FAILED;
		status = -1;
		return false;
	}

	status = 1;
	return true;
}

bool SocialityMgrEngine::OnMsgPresentSong(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();

	std::string user_id, to_user_id, song_id_str, ext_msg;
	if (!packet.GetAttrib("uid", user_id)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("touid", to_user_id)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}

/*
	if (!packet.GetAttrib("songid", song_id_str)) {
		err_code = MIG_FM_HTTP_SONG_ID_NO_VALID;
		return false;
	}
*/
	if (!packet.GetAttrib("msg", ext_msg)) {
	}

	int64 uid = atoll(user_id.c_str());
	if (0 == uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}
	int64 to_uid = atoll(to_user_id.c_str());
	if (0 == to_uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

	//修改为支持多首音乐
	std::string summary;
	if(!PushPresentMsg(ext_msg,summary,user_id,to_user_id,err_code,status))
		return false;

	//加入历史记录
	DBComm::AddMusciFriend(user_id,to_user_id);

	//推送消息配置
	std::string device_token;
	bool is_recv = false;
	unsigned btime=0, etime=0;
	if (!RedisComm::GetUserPushConfig(to_uid, device_token,
		is_recv, btime, etime)) {
		err_code = MIG_FM_DB_READ_PUSH_CONFIG_FAILED;
		status = -1;
		return false;
	}

	if (!is_recv) {
		err_code = MIG_FM_OTHER_PUSH_SERVICE_CLOSED;
		return false;
	}

	time_t cur_time = time(NULL);
	tm cur_tm = *localtime(&cur_time);
	unsigned ct = 60 * cur_tm.tm_hour + cur_tm.tm_min;
	bool enable = true;
	if (btime < etime)
		enable = (btime<=ct) && (ct<=etime);
	else
		enable = (ct<=etime) || (btime<=ct);
	if (!enable) {
		err_code = MIG_FM_OTHER_ANTI_HARASSMENT;
		return false;
	}


	//device_token = "981b5df83c394507ae5b4c13449c826a534f2e01b991e2b9d0641f3414b5b7e3";
	if (!HttpComm::PushMessage(device_token, summary)) {
		err_code = MIG_FM_PUSH_MSG_FAILED;
		status = -1;
		return false;
	}


	status = 1;
	return true;
}

bool SocialityMgrEngine::OnMsgGetPushMsg(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();

	std::string user_id, page_index_str, page_size_str;
	if (!packet.GetAttrib("uid", user_id)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("page_index", page_index_str)) {
		page_index_str = "0";
	}
	if (!packet.GetAttrib("page_size", page_size_str)) {
		page_size_str = "10";
	}

	int64 uid = atoll(user_id.c_str());
	if (0 == uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

	int page_index = atoi(page_index_str.c_str());
	int page_size = atoi(page_size_str.c_str());

	typedef std::list<std::string> MsgList;
	MsgList msg_list;
	if (!RedisComm::GetStagedPushMsg(uid, page_index, page_size, msg_list)) {
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}

	if (msg_list.empty()) {
		err_code = MIG_FM_MSG_LIST_EMPTY;
		return false;
	}

	Json::Value &content = result["result"];
	for (MsgList::iterator it=msg_list.begin(); it!=msg_list.end(); ++it) {
		Json::Value item;
		if (GetPushMsgDetail(user_id,*it, item))
			content.append(item);
	}

	status = 1;
	return true;
}

bool SocialityMgrEngine::OnMsgGetPushMsgSummary(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();
	return true;
}

bool SocialityMgrEngine::OnMsgGetFriendList(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();

	std::string uid_str, page_index_str, page_size_str;
	if (!packet.GetAttrib("uid", uid_str)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
//	if (!packet.GetAttrib("page_index", page_index_str)) {
//		page_index_str = "0";
//	}
//	if (!packet.GetAttrib("page_size", page_size_str)) {
//		page_size_str = "10";
//	}

	int64 uid = atoll(uid_str.c_str());
	if (0 == uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

//	int page_index = atoi(page_index_str.c_str());
//	int page_size = atoi(page_size_str.c_str());

	typedef DBComm::FriendInfoList FriendList;
	FriendList friends;
	if (!DBComm::GetFriendList(uid_str, friends)) {
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}



	status = 1;
	return true;
}

bool SocialityMgrEngine::OnMsgSendFriendMsg(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();
	return true;
}

bool SocialityMgrEngine::OnMsgSayHello(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();
	std::string uid;
	std::string touid;
	std::string msg;
	if (!packet.GetAttrib("uid",uid)){
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("touid",touid)){
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("msg",msg)){

	}
	int64 iuid = atoll(uid.c_str());
	int64 itouid = atoll(touid.c_str());
	if (0>=iuid||0>=itouid){
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}


	int64 msg_id = 0;
	if (!RedisComm::GenaratePushMsgID(atoll(uid.c_str()), msg_id)){
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}

	std::string detail,summary;
	if (!MakeHalloContent(uid,touid,msg_id,msg,detail,summary)){
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}

	if (!RedisComm::StagePushMsg(atoll(touid.c_str()),msg_id,detail)){
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}

	DBComm::AddMusciFriend(uid,touid);


	std::string device_token;
	bool is_recv = false;
	unsigned btime = 0,etime = 0;
	if (!RedisComm::GetUserPushConfig(atoll(touid.c_str()),device_token,is_recv,btime,etime)){
		err_code = MIG_FM_DB_READ_PUSH_CONFIG_FAILED;
		status = -1;
		return false;
	}

	if (!is_recv){
		err_code = MIG_FM_OTHER_PUSH_SERVICE_CLOSED;
		return false;
	}
	time_t cur_time =time(NULL);
	tm cur_tm = *localtime(&cur_time);
	unsigned ct = 60 * cur_tm.tm_hour+cur_tm.tm_min;
	bool enable = true;
	if (btime<etime)
		enable = (btime<=ct) && (ct<=etime);
	else
		enable = (ct<=etime) || (btime<=ct);

	if (!enable){
		err_code = MIG_FM_OTHER_ANTI_HARASSMENT;
		return false;
	}

		//device_token = "981b5df83c394507ae5b4c13449c826a534f2e01b991e2b9d0641f3414b5b7e3";
	if(!HttpComm::PushMessage(device_token,summary)){
		err_code = MIG_FM_PUSH_MSG_FAILED;
		status = -1;
		return false;
	}

	status = 1;
	return true;
}

bool SocialityMgrEngine::OnMsgAddUserBacklist(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();
	return true;
}

bool SocialityMgrEngine::OnMsgAddFriend(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();

	std::string uid_str, touid_str;
	if (!packet.GetAttrib("uid", uid_str)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("touid", touid_str)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}

	int64 uid = atoll(uid_str.c_str());
	if (0 == uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

	int64 to_uid = atoll(touid_str.c_str());
	if (0 == to_uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

//	if (!storage::RedisComm::AddFriend(uid, to_uid)) {
//		err_code = MIG_FM_DB_ACCESS_FAILED;
//		status = -1;
//		return false;
//	}

	if (!DBComm::AddFriend(uid_str, touid_str)) {
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}

	status = 1;
	return true;
}

bool SocialityMgrEngine::OnMsgImportSongList(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();
	return true;
}

bool SocialityMgrEngine::OnMsgImportFriend(packet::HttpPacket& packet,
		Json::Value& result, int& status, int &err_code) {
	LOGIC_PROLOG();
	return true;
}

bool SocialityMgrEngine::OnMsgCommentSong(packet::HttpPacket& packet,
		Json::Value& result, int& status, int& err_code) {
	LOGIC_PROLOG();

	std::string uid_str, songid_str, comment,curjson,tar_uid;
	if (!packet.GetAttrib("uid", uid_str)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("songid", songid_str)) {
		err_code = MIG_FM_HTTP_SONG_ID_NO_VALID;
		return false;
	}
	if (!packet.GetAttrib("comment", comment)) {
		err_code = MIG_FM_HTTP_COMMENT_INVALID;
		return false;
	}

	packet.GetAttrib("tarid",tar_uid);


	int64 uid = atoll(uid_str.c_str());
	if (0 == uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

	int64 songid = atoll(songid_str.c_str());
	if (0 == songid) {
		err_code = MIG_FM_HTTP_COMMENT_INVALID;
		return false;
	}

	//获取这首歌评论数
	this->SetMusicHostCltCmt(songid_str,3,curjson);
	LOG_DEBUG2("%s",curjson.c_str());
	if (!RedisComm::SaveSongComment(songid, uid, comment,curjson)) {
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}

	status = 1;
	return true;
}

bool SocialityMgrEngine::OnMsgGetMusicFriend(packet::HttpPacket& packet,
											 Json::Value &result,
											 int &status, int &err_code){

	LOGIC_PROLOG();
	std::list<struct MusicFriendInfo> user_list;
	std::vector<std::string> vec_users;
	std::string uid_str,fromid_str,count_str;
	std::map<std::string, std::string> temp_usersong;
	std::map<std::string,base::MusicInfo> user_song;
	std::map<std::string,std::string> collect_map;
	if (!packet.GetAttrib("uid", uid_str)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("fromid", fromid_str)) {
		fromid_str = "0";
	}
	if (!packet.GetAttrib("count", count_str)) {
		count_str = "10";
	}

	//获取歌友信息
	DBComm::GetMusicUser(uid_str,fromid_str,count_str,vec_users,user_list);
	if (user_list.size()<0){
		//fix me
		err_code = MIG_FM_HTTP_COMMENT_INVALID;
		return false;
	}

	//获取当前用户试听歌曲
	MemComm::GetUserCurrentSong(vec_users, temp_usersong);

	//获取该用户红心歌曲
	RedisComm::GetCollectSongs(uid_str,collect_map);

	//获取歌曲信息
	RedisComm::GetMusicInfos(temp_usersong,user_song);

	//遍历用户信息 音乐信息 封包
	while(user_list.size()>0){
		Json::Value info;
		struct MusicFriendInfo userinfo = user_list.front();
		user_list.pop_front();
		//获取音乐信息
		GetMusicInfo(info,userinfo.userinfo.uid(),temp_usersong,collect_map,user_song);
		//获取用户信息
		GetUserInfo(info,userinfo);
		result["result"]["nearUser"].append(info);
	}
	status = 1;
	return true;

}

bool SocialityMgrEngine::OnMsgGetComment(packet::HttpPacket& packet,
		Json::Value& result, int& status, int& err_code) {
	LOGIC_PROLOG();

	std::string uid_str, songid_str, count_str, fromid_str;
	if (!packet.GetAttrib("uid", uid_str)) {
		err_code = MIG_FM_HTTP_USER_NO_EXITS;
		return false;
	}
	if (!packet.GetAttrib("songid", songid_str)) {
		err_code = MIG_FM_HTTP_SONG_ID_NO_VALID;
		return false;
	}
	if (!packet.GetAttrib("fromid", fromid_str)) {
		fromid_str = "0";
	}
	if (!packet.GetAttrib("count", count_str)) {
		count_str = "10";
	}

	int64 uid = atoll(uid_str.c_str());
	if (0 == uid) {
		err_code = MIG_FM_HTTP_INVALID_USER_ID;
		return false;
	}

	int64 songid = atoll(songid_str.c_str());
	if (0 == songid) {
		err_code = MIG_FM_HTTP_COMMENT_INVALID;
		return false;
	}

	int64 fromid = atoll(fromid_str.c_str());
	if (fromid < 0) {
		err_code = MIG_FM_HTTP_FROMID_INVALID;
		return false;
	}

	int64 count = atoll(count_str.c_str());
	if (count <= 0) {
		err_code = MIG_FM_HTTP_COUNT_INVALID;
		return false;
	}

	Json::Value &content = result["result"];
	content["songid"] = songid_str;
	Json::Value &comments = content["comments"];
	int64 total = 0;
	if (!RedisComm::ReadSongComment(songid, fromid, count, total, comments)) {
		err_code = MIG_FM_DB_ACCESS_FAILED;
		status = -1;
		return false;
	}

	std::string nickname, gender, type, birthday, location, source, head;
	if (comments.size()!=0){
		for (Json::Value::iterator it = comments.begin(); it!=comments.end(); ++it) {
			Json::Value &item = *it;
			Json::Value &uinfo = item["user"];
			std::string item_uid = item["uid"].asString();
			item.removeMember("uid");
			if (DBComm::GetUserInfos(atoll(item_uid.c_str()), nickname, gender, type, birthday, location, source, head)) {
				uinfo["userid"] = item_uid;
				uinfo["nickname"] = nickname;
				uinfo["gender"] = gender;
				uinfo["pic"] = head;
			}
		}
	}else{
		content["comments"] = 0;
	}

	result["size"] = (unsigned)comments.size();
	result["total"] = (unsigned)total;
	status = 1;
	return true;
}

bool SocialityMgrEngine::CheckAndTransHMTime(const std::string &str, unsigned &time) {
	time = 0;

	if (str.empty())
		return false;

	std::vector<std::string> vec_str;
	if (2 != SomeUtils::SplitStringChr(str.c_str(), ":", vec_str)) {
		return false;
	}

	int hour = atoi(vec_str[0].c_str());
	int minute = atoi(vec_str[1].c_str());

	if (hour<0 || hour>=24)
		return false;
	if (minute<0 || minute>=60)
		return false;

	time = hour*60 + minute;
	return true;
}

bool SocialityMgrEngine::MakeHalloContent(const std::string& send_uid,
										  const std::string& to_uid, int64 msg_id,
										  const std::string& msg,std::string& detail, 
										  std::string &summary){
   detail.clear();
   summary.clear();
   std::string sd_nick, sd_sex, sd_head;
   std::string to_nick, to_sex, to_head;

   if (!DBComm::GetUserInfos(send_uid, sd_nick, sd_sex, sd_head))
	   return false;
   if (!DBComm::GetUserInfos(to_uid, to_nick, to_sex, to_head))
	   return false;

   std::stringstream ss;
   ss << sd_nick << "(" << send_uid << ")" << "向你打招呼";
   summary.assign(ss.str());

   char tmp[256] = {0};
   Json::FastWriter wr;
   Json::Value value;
   value["action"] = "sayhello";
   snprintf(tmp, arraysize(tmp), "%lld", msg_id);
   value["msgid"] = tmp;
   Json::Value &content = value["content"];
   content["send_uid"] = send_uid.c_str();
   content["to_uid"] = to_uid.c_str();
   content["msg"] = msg;
   std::string cur_time;
   SomeUtils::GetCurrntTimeFormat(cur_time);
   content["time"] = cur_time;
   detail = wr.write(value);
   return true;

}

bool SocialityMgrEngine::MakePresentSongContent(const std::string &send_uid, 
												const std::string &to_uid, 
												const std::string &song_id,
												int64 msg_id,
												const std::string &msg, 
												std::string &detail){
    bool r = false;
	char tmp[256] = {0};
	Json::FastWriter wr;
	Json::Value value;

	//获取歌曲信息，杜绝不存在歌曲id提交
	if(!GetMusicInfos(to_uid,song_id,value["song"]))
		return false;


	value["action"] = "presentsong";
	snprintf(tmp, arraysize(tmp), "%lld", msg_id);
	value["msgid"] = tmp;
	Json::Value &content = value["content"];
	content["send_uid"] = send_uid.c_str();
	content["to_uid"] = to_uid.c_str();
	content["msg"] = msg;
	std::string cur_time;
	SomeUtils::GetCurrntTimeFormat(cur_time);
	content["time"] = cur_time;
	detail = wr.write(value);
	return true;
}


bool SocialityMgrEngine::MakePresentSongContent(const std::string& send_uid,
												const std::string& to_uid,
												const std::string& song_id,
												int64 msg_id,
												const std::string& msg,
												std::string &detail, 
												std::string &summary) {
	detail.clear();
	summary.clear();
	bool r = false;

	std::string sd_nick, sd_sex, sd_head;
	std::string to_nick, to_sex, to_head;
	base::UserInfo sd_usrinfo;
	base::UserInfo to_usrinfo;
	if (!base::BasicUtil::GetUserInfo(send_uid,sd_usrinfo))
		return false;
	if (!base::BasicUtil::GetUserInfo(to_uid,to_usrinfo))
		return false;

	//if (!DBComm::GetUserInfos(send_uid, sd_nick, sd_sex, sd_head))
	//	return false;
	//if (!DBComm::GetUserInfos(to_uid, to_nick, to_sex, to_head))
	//	return false;

	std::stringstream ss;
	ss << sd_usrinfo.nickname().c_str() << "(" << send_uid << ")" << "赠送您一首歌";
	summary.assign(ss.str());

	

	char tmp[256] = {0};
	Json::FastWriter wr;
	Json::Value value;
	
	//获取歌曲信息，杜绝不存在歌曲id提交
 	if(!GetMusicInfos(to_uid,song_id,value["song"]))
 		return false;

	value["action"] = "presentsong";
	snprintf(tmp, arraysize(tmp), "%lld", msg_id);
	value["msgid"] = tmp;
	Json::Value &content = value["content"];
	content["send_uid"] = send_uid.c_str();
	content["to_uid"] = to_uid.c_str();
	content["msg"] = msg;
	std::string cur_time;
	SomeUtils::GetCurrntTimeFormat(cur_time);
	content["time"] = cur_time;
	detail = wr.write(value);

	return true;
}


bool SocialityMgrEngine::GetPresentSongDetail(const std::string& uid,
											  Json::Value& content) {
	std::string song_id = content["song_id"].asString();


	if (!GetMusicInfos(uid,song_id, content["song"])) {
		return false;
	}

	return true;
}

bool SocialityMgrEngine::GetPushMsgDetail(const std::string& uid,
										  const std::string& msg,
		Json::Value& content) {
	bool r = false;
	content.clear();

	Json::Reader rd;
	Json::Value root;
	if(!rd.parse(msg.c_str(), root)) {
		return false;
	}

	content["type"] = root["action"];
	Json::Value& detail_node = content["detail"] = root["content"];
	std::string tar_uid = detail_node["send_uid"].asString();

	std::string type = content["type"].asString();
	if (type == "presentsong") {
		//r = GetPresentSongDetail(uid,detail_node);
		//content["song"] = root["song"];
		content["music"] = root["song"];
	}else {
		// do nothing
	}
	//userinfo
	base::UserInfo usrinfo;
	r = base::BasicUtil::GetUserInfo(tar_uid,usrinfo);
	if (r){
		Json::Value& userjson = content["userinfo"];
		userjson["userid"] = usrinfo.uid();
		userjson["nickname"] =  usrinfo.nickname();
		userjson["sex"] = usrinfo.sex();
		userjson["head"] = usrinfo.head();
		userjson["birthday"] = usrinfo.birthday();
		userjson["location"] = usrinfo.crty();
		userjson["source"] = usrinfo.source();
	}
	return true;
}

bool SocialityMgrEngine::GetMusicInfos(const std::string& uid,
									   const std::string& songid, 
									   Json::Value &info) {
	std::stringstream os;
	std::stringstream os1;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	std::string mode;
	std::string wordid;
	std::string dec;
	std::string dec_id;
	std::string dec_word;
	std::string music_info;
	std::string content_url;
	std::string hi_content_url;
	std::string hot_num;
	std::string cmt_num;
	std::string clt_num;
	base::MusicInfo smi;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	std::string is_like;
	bool r = false;

	r = RedisComm::GetMusicInfos(songid,music_info);

	if (!r)
		return false;

	r = smi.UnserializedJson(music_info);
	if (!r)
		return false;
	MIG_DEBUG(USER_LEVEL,"artist[%s] title[%s]",smi.artist().c_str(),
		smi.title().c_str());

	//DBComm::GetWXMusicUrl(smi.id(),content_url,dec,dec_id,dec_word);
	DBComm::GetMusicUrl(smi.id(),hi_content_url,content_url);
	smi.set_url(content_url);
	smi.set_hq_url(hi_content_url);
	Base64Decode(smi.title(),&b64title);
	Base64Decode(smi.artist(),&b64artist);
	Base64Decode(smi.album_title(),&b64album);

//是否是红心歌曲
	r = RedisComm::IsCollectSong(uid,songid);
	if (r)
		is_like = "1";
	else
		is_like = "0";

	r = this->GetMusicHotCltCmt(songid,hot_num,cmt_num,clt_num);

	info["id"] = smi.id();
	info["title"] = b64title;
	info["artist"] = b64artist;
	info["url"] = smi.url();
	info["hqurl"] = smi.hq_url();
	info["pub_time"] = smi.pub_time();
	info["album"] = b64album;
	info["pic"] = smi.pic_url();
	info["like"] = is_like;
	info["clt"] = clt_num;
	info["cmt"] = cmt_num;
	info["hot"] = hot_num;
	info["id"] = smi.id();

	return true;
}

bool SocialityMgrEngine::SetMusicHostCltCmt(const std::string &songid, const int32 flag, 
											std::string &json)
{
	std::string hot_num;
	std::string cmt_num;
	std::string clt_num;
	std::stringstream os;
	int64 refcount;
	bool r = false;
	r = this->GetMusicHotCltCmt(songid,hot_num,cmt_num,clt_num);

	//意外情况
	if (hot_num.empty())
		hot_num="0";
	if (cmt_num.empty())
		cmt_num="0";
	if (clt_num.empty())
		clt_num="0";

	if (!r){//第一次添加
		switch (flag){
		  case 1:
			  hot_num = "1";
			  cmt_num = clt_num= "0";
			  break;  
		  case 2:
			  clt_num = "1";
			  hot_num = cmt_num= "0";
			  break;
		  case 3:
			  cmt_num = "1";
			  clt_num = hot_num= "0";
			  break;
		  default:
			  clt_num = cmt_num = hot_num = "0";
		}
	}else{//累计累加
		switch (flag){
		  case 1:
			  refcount = atoll(hot_num.c_str());
			  refcount++;
			  os<<refcount;
			  hot_num = os.str();
			  break;  
		  case 2:
			  refcount = atoll(clt_num.c_str());
			  refcount++;
			  os<<refcount;
			  clt_num = os.str();
			  break;
		  case 3:
			  refcount = atoll(cmt_num.c_str());
			  refcount++;
			  os<<refcount;
			  cmt_num = os.str();
			  break;
		}
	}

	json.append("{\"hot\":\"");
	json.append(hot_num.c_str());
	json.append("\",\"cmt\":\"");
	json.append(cmt_num.c_str());
	json.append("\",\"clt\":\"");
	json.append(clt_num.c_str());
	json.append("\"}");

	return true;
}


bool SocialityMgrEngine::GetMusicHotCltCmt(const std::string &songid, 
									   std::string &hot_num, 
									   std::string &cmt_num, 
									   std::string &clt_num){
	   std::string content;
	   bool r = false;
	   Json::Reader reader;
	   Json::Value  root;
	   Json::Value songinfo;
	   r = mig_sociality::RedisComm::GetMusicAboutUser(songid,content);
	   if (!r){
		   hot_num = clt_num = cmt_num = "0";
		   return false;
	   }

	   //value {"hot":"123","cmt":"231",clt:"2312"}
	   r = reader.parse(content.c_str(),root);
	   if (!r)
		   return false;
	   if (root.isMember("hot"))
		   hot_num = root["hot"].asString();
	   else
		   hot_num = "0";

	   if (root.isMember("cmt"))
		   cmt_num = root["cmt"].asString();
	   else
		   cmt_num = "0";

	   if (root.isMember("clt"))
		   clt_num = root["typeid"].asString();
	   else
		   clt_num = "0";
	   return true;
}

bool SocialityMgrEngine::GetUserInfo(Json::Value &value,
                                     struct MusicFriendInfo& userinfo){
	value["userinfo"]["latitude"] = userinfo.latitude;
	value["userinfo"]["distance"] = userinfo.distance;
	value["userinfo"]["longitude"] = userinfo.longitude;
	value["userinfo"]["nickname"] = userinfo.userinfo.nickname();
	value["userinfo"]["sex"] = userinfo.userinfo.sex();
	value["userinfo"]["userid"] = userinfo.userinfo.uid();
	value["userinfo"]["head"] = userinfo.userinfo.head();
	value["userinfo"]["plat"] = userinfo.userinfo.source();
	return true;
}

bool SocialityMgrEngine::GetMusicInfo(Json::Value &value,const std::string& uid,
		std::map<std :: string,std :: string> &temp_usersong, 
		std::map<std :: string,std :: string> &collect_map, 
		std::map<std :: string,base :: MusicInfo> &user_song){
   
	bool r  = false;
    //获取音乐信息
	std::map<std :: string,base :: MusicInfo>::iterator it_music = user_song.find(uid);
	std::map<std :: string,std :: string>::iterator it_temp = temp_usersong.find(uid);
	if (it_music==user_song.end())
		return false;
	if (it_temp == temp_usersong.end())
		return false;
	base::MusicInfo music_info = it_music->second;
	std::string temp_info = it_temp->second;

	/*从temp_usersong 获取状态 
	value {"songid":"10000","state":"1","type":"mm","tid":"1","name":"艳阳天","singer":"窦唯"}
	*/

	std::string state;
	std::string tid;
	std::string type;
	Json::Reader reader;
	Json::Value  root;
	std::string hot_num;
	std::string clt_num;
	std::string cmt_num;
	std::string hq_content_url;
	std::string content_url;
	bool is_like = false;
	r = reader.parse(temp_info,root);
	if (r){
		if (root.isMember("state")){
			state = root["state"].asString();
		}
		if (root.isMember("tid")){
			tid = root["tid"].asString();
		}
		if (root.isMember("type")){
			type = root["type"].asString();
		}
	}

	//是否是红心歌曲
	std::map<std :: string,std :: string>::iterator it_like = 
		collect_map.find(music_info.id());

	if (it_like!=collect_map.end())
		is_like = true;

	GetMusicHotCltCmt(music_info.id(),hot_num,cmt_num,clt_num);

	//storage::DBComm::GetMusicUrl(music_info.id(),hq_content_url,content_url);
	mig_sociality::DBComm::GetMusicUrl(music_info.id(),hq_content_url,content_url);

	music_info.set_hq_url(hq_content_url);
	music_info.set_url(content_url);

	std::string b64title,b64artist,b64album;
	Base64Decode(music_info.title(),&b64title);
	Base64Decode(music_info.artist(),&b64artist);
	Base64Decode(music_info.album_title(),&b64album);

	//组装json
	value["songstat"] = state;
	value["music"]["album"] = b64album;
	value["music"]["artist"] = b64artist;
	value["music"]["title"] = b64title;
	value["music"]["clt"] = clt_num;
	value["music"]["cmt"] = cmt_num;
	value["music"]["hot"] = hot_num;
	value["music"]["hqurl"] = music_info.hq_url();
	value["music"]["id"] = music_info.id();
	if (is_like)
		value["music"]["like"] = 1;
	else
		value["music"]["like"] = 0;
	value["music"]["pic"] = music_info.pic_url();
	value["music"]["pub_time"] = music_info.pub_time();
	value["music"]["tid"] = tid;
	value["music"]["type"] = type;
	value["music"]["url"] = content_url;
	return true;
}

bool SocialityMgrEngine::PushPresentMsg(std::string &msg, std::string& summary,
										std::string& uid,std::string& to_uid,
										int& err_code,int& status){
   
    //解析json
/*

{
"song": [
{
"songid": "234",
"msg": "来听听"
},
{
"songid": "2334",
"msg": "来听听下嘛"
}
]
}
*/
	Json::Reader reader;
	Json::Value  root;
	std::string sd_nick, sd_sex, sd_head;
	std::string to_nick, to_sex, to_head;
	base::UserInfo sd_usrinfo;
	base::UserInfo to_usrinfo;

	if (!base::BasicUtil::GetUserInfo(uid,sd_usrinfo))
		return false;
	if (!base::BasicUtil::GetUserInfo(to_uid,to_usrinfo))
		return false;

	bool r = reader.parse(msg,root);
	if (!r){
		err_code = MIG_FM_HTTP_JSON_ERROR;
		status = -1;
		return false;
	}
	if (!root.isMember("song")){
		err_code = MIG_FM_HTTP_JSON_ERROR;
		status = -1;
		return false;
	}

	int32 infos_size = root["song"].size();
	if (infos_size<=0){
		err_code = MIG_FM_HTTP_JSON_ERROR;
		status = -1;
		return false;
	}

	Json::Value song = root["song"];

	for (int i = 0;i<infos_size;i++){
		if (!song[i].isMember("songid"))
			continue;
		if (!song[i].isMember("msg"))
			continue;

		std::string id = song[i]["songid"].asString();
		std::string msg = song[i]["msg"].asString();
		std::string detail, summary;
		int64 msg_id = 0;
		if (!RedisComm::GenaratePushMsgID(atoll(id.c_str()), msg_id)) {
			err_code = MIG_FM_DB_ACCESS_FAILED;
			status = -1;
			return false;
		}

		if (!MakePresentSongContent(uid, to_uid, id, msg_id, msg,
			detail)) {
				err_code = MIG_FM_DB_ACCESS_FAILED;
				status = -1;
				return false;
		}

		if (!RedisComm::StagePushMsg(atoll(to_uid.c_str()), msg_id, detail)) {
			err_code = MIG_FM_DB_ACCESS_FAILED;
			status = -1;
			return false;
		}

	}

	//内容
	std::stringstream ss;
	std::string conver_num;
	r = base::BasicUtil::ConverNum(infos_size,conver_num);
	if(!r)
		conver_num.append("0");
	ss << sd_usrinfo.nickname().c_str() << "(" << uid << ")" << "赠送您"<<conver_num<<"首歌";
	summary.assign(ss.str());
	return true;
}

}
