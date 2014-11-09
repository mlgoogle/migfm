#include "http_api.h"
#include "api_errno.h"
#include "basic/constants.h"


#define PARAM_IS_EXIST_RETURN(NAME,PARAM,ERR_CODE) \
	r = packet.GetAttrib(NAME,PARAM); \
	if(!r||PARAM.empty()){\
		err = ERR_CODE;\
		return false;\
	}\


#define PARAM_IS_EXIST_DEFAULTPARAM(NAME,PARAM,DEFAULT)\
	r = packet.GetAttrib(NAME,PARAM); \
	if(!r||PARAM.empty()){\
		if(!DEFAULT.empty())\
			PARAM = DEFAULT;\
	}\




namespace base_net{

//注册接口
bool MYHttpApi::UserManager::OnUserRegister(packet::HttpPacket& packet,std::string& username,
		std::string& password,std::string& nickname,std::string& source,
		std::string& session,std::string& sex,std::string& birthday,std::string& location,
		std::string& address,std::string& head,int& err){
	bool r = false;
	std::string default_session = "1";
	std::string default_username = "default@miglab.com";
	std::string default_nickname = "米格用户";
	std::string default_sex = "1";
	std::string default_birthday= "1986-10=01";
	std::string default_location;
	std::string default_head;
	std::string default_address;//通过提交的或者坐标来判断城市
	PARAM_IS_EXIST_RETURN("source",source,SOURCE_NOT_EXIST);
	//不为本平台即source 为0 则判断session
	if(source!="0"){
		PARAM_IS_EXIST_RETURN("session",session,SESSION_NOT_EXIST);
	}else{
		PARAM_IS_EXIST_DEFAULTPARAM("session",session,default_session);
	}

	//第三方平台用户名为空 密码为空 设置默认用户密码，具体到数据来分配
	if(source=="0"){
		PARAM_IS_EXIST_RETURN("username",username,USERNAME_NOT_EXIST);
		PARAM_IS_EXIST_RETURN("password",password,PASSWORD_NOT_EXIST);
		//本平台用户默认性别男
		PARAM_IS_EXIST_DEFAULTPARAM("sex",sex,default_sex);
		PARAM_IS_EXIST_DEFAULTPARAM("location",location,default_location);
		PARAM_IS_EXIST_DEFAULTPARAM("head",head,default_head);

	}else{
		PARAM_IS_EXIST_DEFAULTPARAM("username",username,default_username);
		PARAM_IS_EXIST_DEFAULTPARAM("password",password,default_username);
	}
	PARAM_IS_EXIST_DEFAULTPARAM("nickname",nickname,default_nickname);
	PARAM_IS_EXIST_DEFAULTPARAM("address",address,default_address);
	return true;
}

//登陆接口
bool MYHttpApi::UserManager::OnUserLogin(packet::HttpPacket& packet,std::string& username,
				std::string& password,std::string& clientid,int& err){

	bool r = false;
	PARAM_IS_EXIST_RETURN("username",username,USERNAME_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("password",password,PASSWORD_NOT_EXIST);

	return true;
}

//更新信息接口
bool MYHttpApi::UserManager::OnUserUpdate(packet::HttpPacket& packet,std::string& uid,
        std::string& nickname,std::string& gender,std::string& birthday,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("nickname",nickname,NICKNAME_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("gender",gender,SEX_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("birthday",birthday,BIRTHDAY_NOT_EXIST);
	return true;
}

/*
bool MYHttpApi::UserManager::OnBindPush(packet::HttpPacket& packet,std::string& uid,std::string& channel,std::string& userid,
		int& err){
	bool r = false;
	std::string token;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("token",token,TOKEN_EXIST);
	PARAM_IS_EXIST_RETURN("channelid",channel,CHANNLE_EXIST);
	PARAM_IS_EXIST_RETURN("userid",userid,CHANNLE_EXIST);
	return true;
}
*/

bool MYHttpApi::ChatManager::OnGetBestIdle(packet::HttpPacket& packet,std::string& platformid,int& err){
	bool r = false;
	std::string default_platformid = "10000";
	PARAM_IS_EXIST_DEFAULTPARAM("platformid",platformid,default_platformid);
	return true;
}

bool MYHttpApi::ChatManager::OnGetHisChat(packet::HttpPacket& packet,std::string& platformid,std::string& uid,
		std::string& tid,std::string& msgid,std::string& from,std::string& count,int& err){
	bool r = false;
	std::string default_platformid = "10000";
	std::string default_from = "0";
	std::string default_count = "5";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("tid",tid,TID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("msgid",tid,MSGID_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("platformid",platformid,default_platformid);
	PARAM_IS_EXIST_DEFAULTPARAM("from",from,default_from);
	PARAM_IS_EXIST_DEFAULTPARAM("count",count,default_count);
	return true;
}



bool MYHttpApi::MusicManager::OnGetChannel(packet::HttpPacket& packet,std::string& num,int& err){
	bool r = false;
	std::string default_num = "5";
	PARAM_IS_EXIST_DEFAULTPARAM("num",num,default_num);
	return true;
}

bool MYHttpApi::MusicManager::OnGetChannelSong(packet::HttpPacket& packet,std::string& channel,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("channel",channel,CHANNEL_ID_NOT_EXIST);
	return true;
}


bool MYHttpApi::MusicManager::OnGetMoodAndScene(packet::HttpPacket& packet,std::string& mode,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("deword",mode,MODE_NOT_EXIST);
	return true;
}


bool MYHttpApi::MusicManager::OnGetMoodAndSceneMusic(packet::HttpPacket& packet,std::string& mode,std::string tid
		,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("mode",mode,MODE_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("wordid",mode,MODE_ID_NOT_EXIST);
	return true;
}


bool MYHttpApi::MusicManager::OnGetSongInfo(packet::HttpPacket& packet,std::string& songid,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	return true;
}


bool MYHttpApi::MusicManager::OnGetLyric(packet::HttpPacket& packet,std::string& songid,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	return true;
}


bool MYHttpApi::MusicManager::OnGetHandselSong(packet::HttpPacket& packet,std::string& uid,std::string& fromid,
		std::string& count,std::string& islike,int& err){
	bool r = false;
	std::string default_fromid = "0";
	std::string default_count = "10";
	std::string default_islike = "0";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("fromid",fromid,default_fromid);
	PARAM_IS_EXIST_DEFAULTPARAM("count",count,default_count);
	PARAM_IS_EXIST_DEFAULTPARAM("islike",islike,default_islike);
	return true;
}


bool MYHttpApi::MusicManager::OnGetUpdateconfigFile(packet::HttpPacket& packet,
		std::string& version,std::string& uid,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("version",version,UID_NOT_EXIST);
	return true;
}

bool MYHttpApi::MusicManager::OnGetTypeSongs(packet::HttpPacket& packet,std::string& uid,std::string& tid,
				std::string& typeindex,std::string& moodid,std::string& moodindex,std::string& sceneid,
				std::string& sceneindex,std::string& num,int& err){
	bool r = false;
	std::string default_num = "5";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("typeid",tid,MODE_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("typeindex",typeindex,TYPE_INDEX_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("moodid",moodid,MOOD_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("moodindex",moodindex,MOOD_INDEX_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("sceneid",sceneid,SCENCE_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("sceneindex",sceneindex,SCENE_INDEX_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("num",num,default_num);
	return true;
}

bool MYHttpApi::MusicManager::OnGetCollectionSong(packet::HttpPacket& packet,std::string& uid,std::string& tid
		,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("taruid",tid,TAR_UID_NOT_EXIST);
	return true;
}

bool MYHttpApi::MusicManager::OnSubmitCurrentListenSongs(packet::HttpPacket& packet,std::string& uid,
				std::string& mode,std::string& type_id,std::string& cur_song,std::string& last_song,
				std::string& name,std::string& singer,std::string& state,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("mode",mode,MODE_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("typeid",type_id,MODE_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("cursong",cur_song,CUR_SONG_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("lastsong",last_song,LAST_SONG_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("name",name,NAME_SONG_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("singer",singer,SINGER_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("state",state,STATE_NOT_EXIST);
	return true;
}

bool MYHttpApi::MusicManager::OnSubmitDelHateSongs(packet::HttpPacket& packet,std::string& uid,std::string& songid
		,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	return true;
}

bool MYHttpApi::MusicManager::OnSubmitDelCollectionSongs(packet::HttpPacket& packet,std::string& uid,
				std::string& songid,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	return true;
}

bool MYHttpApi::MusicManager::OnSubmitAddHateSongs(packet::HttpPacket& packet,std::string& uid,
		std::string& songid,std::string& modetype,std::string& type_id,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("modetype",modetype,MODE_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("typeid",type_id,MODE_ID_NOT_EXIST);
	return true;
}

bool MYHttpApi::MusicManager::OnSubmitAddCollectionSongs(packet::HttpPacket& packet,std::string& uid,
		std::string& songid,std::string& modetype,std::string& type_id,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("modetype",modetype,MODE_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("typeid",type_id,MODE_ID_NOT_EXIST);
	return true;
}


bool MYHttpApi::MusicManager::OnSubmitShareResult(packet::HttpPacket& packet,std::string& uid,
		std::string& songid,std::string& share_plat,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("share_plat",share_plat,PLAT_ID_NOT_EXIST);
	return true;
}

bool MYHttpApi::LbsManager::OnSetUserPos(packet::HttpPacket& packet,std::string& uid,std::string& location,
		int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("location",location,UID_NOT_EXIST);
	return true;

}

bool MYHttpApi::LbsManager::OnNearSet(packet::HttpPacket& packet,std::string& uid,std::string& location,
				std::string& page_index,std::string& page_size,int& err){
	bool r = false;
	std::string default_index = "0";
	std::string default_size = "10";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("location",location,UID_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("page_index",page_index,default_index);
	PARAM_IS_EXIST_DEFAULTPARAM("page_size",default_index,default_index);
	return true;
}

bool MYHttpApi::Sociality::OnPresentSong(packet::HttpPacket& packet,std::string& uid,std::string& tuid,
		std::string& ext_msg,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("touid",uid,TID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("msg",ext_msg,EXT_MSG_NOT_EXIST);
	return true;
}

bool MYHttpApi::Sociality::OnGetMsgPush(packet::HttpPacket& packet,std::string& uid,std::string& page_index,
		std::string& page_size,int& err){
	bool r = false;
	std::string default_index = "0";
	std::string default_size = "10";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("page_index",page_index,default_index);
	PARAM_IS_EXIST_DEFAULTPARAM("page_size",default_index,default_index);
	return true;
}

bool MYHttpApi::Sociality::OnGetFriendList(packet::HttpPacket& packet,std::string& uid,std::string& page_index,
		std::string& page_size,int& err){
	return true;
}

bool MYHttpApi::Sociality::OnSayHello(packet::HttpPacket& packet,std::string& uid,std::string& tuid,
		std::string& ext_msg,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("touid",tuid,TID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("msg",ext_msg,EXT_MSG_NOT_EXIST);
	return true;
}

bool MYHttpApi::Sociality::OnMsgCommentSong(packet::HttpPacket& packet,std::string& uid,std::string& songid,
		std::string& comment,std::string& tid,int& err){
	bool r = false;
	std::string default_tid = "0";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("comment",comment,COMMENT_MSG_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("tarid",tid,default_tid);
	return true;
}

bool MYHttpApi::Sociality::OnGetComment(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& from,std::string& count,int& err){
	bool r = false;
	std::string default_from = "0";
	std::string default_count = "10";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("page_index",from,default_from);
	PARAM_IS_EXIST_DEFAULTPARAM("page_size",count,default_count);
	return true;
}

bool MYHttpApi::Sociality::OnGetMusicUser(packet::HttpPacket& packet,std::string& uid,std::string& from,
				std::string& count,int& err){
	bool r = false;
	std::string default_from = "0";
	std::string default_count = "10";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("page_index",from,default_from);
	PARAM_IS_EXIST_DEFAULTPARAM("page_size",count,default_count);
	return true;
}

bool MYHttpApi::Sociality::OnGetShareInfo(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& plat,std::string& mode,std::string& index,std::string& latitude,
				std::string& longitude,int& err){
	bool r = false;
	std::string default_latitude = "30.2936";
	std::string default_longitude = "120.1614";
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("plat",plat,PLAT_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("mode",mode,MODE_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("index",index,MODE_ID_NOT_EXIST);
	PARAM_IS_EXIST_DEFAULTPARAM("latitude",latitude,default_latitude);
	PARAM_IS_EXIST_DEFAULTPARAM("longitude ",longitude,default_longitude);
	return true;
}

bool MYHttpApi::Sociality::OnGiftLuckMessage(packet::HttpPacket& packet,std::string& uid,std::string& songid,
				std::string& plat,std::string& mode,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("songid",songid,SONG_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("plat",plat,PLAT_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("mode",mode,MODE_ID_NOT_EXIST);
	return true;
}

}




#undef PARAM_IS_EXIST_RETURN
#undef PARAM_IS_EXIST_DEFAULTPARAM
