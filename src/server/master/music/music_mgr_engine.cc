#include "music_mgr_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "music_cache_mgr.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include "json/json.h"
#include "basic/base64.h"
#include "basic/errno_comm.h"
#include "algorithm/algorithm_base.h"
#include <sstream>

#define		TIME_TEST		1025

namespace music_logic{


MusicMgrEngine::MusicMgrEngine(){
	Init();
}

bool MusicMgrEngine::Init(){
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	usr_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}
	r = config->LoadConfig(path);
	storage::DBComm::Init(config->mysql_db_list_);
	storage::MemComm::Init(config->mem_list_);
	storage::RedisComm::Init(config->redis_list_);
	music_logic::CacheManagerOp::GetMusicCache();

	get_song_engine_ = music_logic::GetSongUrl::Create(music_logic::TYPE_SOGOU);
	if (get_song_engine_==NULL){
		MIG_ERROR(USER_LEVEL,"get_song create error");
		return false;
	}
	std::string get_song_url = "http://121.199.32.88/getmusicurl.ashx";
	get_song_engine_->Init(get_song_url);
	//std::string dir = "./";
	std::string recording_path = "mood_recording.log";
	std::string usr_local_music_path = "usr_local_music.xml";
	config->mood_path_ = "/home/mood/";
	config->usr_local_music_path_ = "/home/usr_local/";
	mood_record_engine_ =
		new music_record::MoodRecordingEngine(config->mood_path_,
		                                      recording_path);
	user_local_music_engine_ = 
		new music_record::UserLocalMusicRecodingEngine(config->usr_local_music_path_,
		                                               usr_local_music_path);    
}

MusicMgrEngine::~MusicMgrEngine(){
	if (mood_record_engine_){
		delete mood_record_engine_;
		mood_record_engine_ = NULL;
	}
	if (user_local_music_engine_){
		delete user_local_music_engine_;
		user_local_music_engine_ = NULL;
	}
}

MusicMgrEngine* MusicMgrEngine::instance_ = NULL;
MusicMgrEngine* MusicMgrEngine::GetInstance(){
   if (instance_==NULL){
        instance_ = new MusicMgrEngine();
   }

   return instance_;
}

void MusicMgrEngine::FreeInstance(){
	delete instance_;
}

bool MusicMgrEngine::OnBroadcastClose(struct server *srv, int socket){
	return true;
}

bool MusicMgrEngine::OnBroadcastConnect(struct server *srv, int socket, 
									  void *data, int len){
    return true;
}

bool MusicMgrEngine::OnBroadcastMessage(struct server *srv, int socket, 
										void *msg, int len){
    return true;
}

bool MusicMgrEngine::OnIniTimer(const struct server *srv){
	//srv->add_time_task(srv, "user_manager", TIME_TEST, 300, 1);
    return true;
}

bool MusicMgrEngine::OnTimeout(struct server *srv, char *id, int opcode, int time){

	return true;
}

bool MusicMgrEngine::OnMusicMgrClose(struct server *srv,
									 int socket){
    return true;
}

bool MusicMgrEngine::OnMusicMgrConnect(struct server *srv, int socket){
    return true;
}

bool MusicMgrEngine::OnMusicMgrMessage(struct server *srv, int socket, 
								       const void *msg, int len){

    const char* packet_stream = (char*)(msg);
	packet::HttpPacket packet(packet_stream,len-1);
	std::string type;
	packet.GetPacketType(type);
	if (type=="getcl"){
		GetMusicChannel(socket,packet);
	}else if(type=="getsong"){
		GetMusicChannelSong(socket,packet);
	}else if (type=="word"){
		GetDescriptionWord(socket,packet);
	}else if (type=="getmssong"){
		GetMoodSceneWordSong(socket,packet);
	}else if (type=="getsongid"){
		GetWXMusicInfo(socket,packet);
	}else if (type=="getmmap"){
		GetMoodMap(socket,packet);
	}else if (type=="getmparent"){
		GetMoodParent(socket,packet);
	}else if (type=="setcltsong"){//记录收藏度
		PostCollectAndHateSong(socket,packet,1);
	}else if (type=="getcltsongs"){
		GetSongList(socket,packet,1);
	}else if (type=="delcltsong"){
		DelCollectAndHateSong(socket,packet,1);
	}else if (type=="sethtsong"){
		PostCollectAndHateSong(socket,packet,0);
	}else if (type=="delthsong"){
		DelCollectAndHateSong(socket,packet,0);
	}else if (type=="recordcursong"){//记录热度
		SetMoodRecording(socket,packet);
	}else if (type=="getdefsongs"){
		GetSongList(socket,packet,0);
	}else if (type=="dbgetsong"){
		GetDoubanMusicChannelSong(socket,packet);
	}else if (type=="gettypesongs"){
		GetTypeSongs(socket,packet);
	}else if (type=="recordlocalsongs"){
		PostUserLocalMusicinfos(socket,packet);
	}else if (type=="updateconfigfile"){
		UpdateConfigFile(socket,packet);
	}else if (type=="getuserhis"){
		GetUserMusicCltAndHis(socket,packet);
	}
    return true;
}

bool MusicMgrEngine::GetMoodParent(const int socket,const packet::HttpPacket& packet){
	packet::HttpPacket pack = packet;
	std::string result;
	std::string result_out;
	std::string status;
	std::string msg;
	bool r =false;
	std::stringstream os;
	int32 utf8_flag = 0;
	int32 index = 0;
	std::string b64word;
	std::list<base::WordAttrInfo> word_list;
	std::list<base::WordAttrInfo>::iterator it;
	//获取心情词
	r = storage::DBComm::GetMoodParentWord(word_list);
	if (!r){
		status = "1";
		//msg = "心情不存在;";
		msg = migfm_strerror(MIG_FM_MOOD_NO_VALID);
		utf8_flag = 0;
		goto ret;
	}

	os<<"\"mood\":[{";
	 it = word_list.begin();
	Base64Decode((*it).name(),&b64word);
	os<<"\"typeid\":\""<<(*it).id().c_str()
		<<"\",\"name\":\""<<b64word.c_str()<<"\"}";

	word_list.pop_front();

	while (word_list.size()>0){
		base::WordAttrInfo dec_word = word_list.front();
		index++;
		Base64Decode(dec_word.name(),&b64word);
		os<<",{\"typeid\":\""<<dec_word.id().c_str()
			<<"\",\"name\":\""<<b64word.c_str()<<"\"}";
		word_list.pop_front();
	}

	os<<"]";
	result = os.str();
	status = "1";
	msg = "0";
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
}

bool MusicMgrEngine::GetMoodMap(const int socket,const packet::HttpPacket& packet){
	
	packet::HttpPacket pack = packet;
	std::string uid;
	std::string result;
	std::string result_out;
	std::string status;
	std::string msg;
	int32 utf8_flag = 0;
	bool r = pack.GetAttrib(UID,uid);
	if (!r){
		LOG_ERROR("Get UID Error");
		status = "1";
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		utf8_flag = 0;
		goto ret;
	}

	//从redis 里面获取用户心绪图
	//音乐机器人专门从操作记录里面分析
	r = storage::RedisComm::GetUserMoodMap(uid,result);
	if (result.empty()){
		//LOG_ERROR("Get UID Error");
		status = "1";
		msg = migfm_strerror(MIG_FM_USER_MOOD_NO_EXITS);
		utf8_flag = 0;
		goto ret;
	}
	status = "1";
	msg = "0";
	utf8_flag = 0;
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
}

bool MusicMgrEngine::GetWXMusicInfo(const int socket, 
									const packet::HttpPacket &packet){
	packet::HttpPacket pack = packet;
	std::string songid;
	bool r = pack.GetAttrib(SONGID,songid);
	if (!r){
		LOG_ERROR("get SONGID error");
		return false;
	}

	return GetMusicInfos(socket,songid);

}

bool MusicMgrEngine::GetMoodSceneWordSong(const int socket,
										  const packet::HttpPacket& packet){
	
	packet::HttpPacket pack = packet;
	int32 utf8_flag = 0;
	std::stringstream os;
	std::stringstream os1;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	std::string mode;
	std::string wordid;
	std::string music_info;
	std::string content_url;
	std::string hq_content_url;
	std::string num;
	base::MusicInfo smi;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	std::string song_id;
	std::string uid;
	int is_like = 0;
	int current_num;
	int flag = 0;
	std::map<std::string, std::string> songid_map;
	std::map<std::string, std::string>::iterator it;
	bool r = pack.GetAttrib(MODE,mode);
	if (!r){
		//LOG_ERROR("get MODE error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_MODE_NO_VALID);
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(WORDID,wordid);
	if (!r){
		LOG_ERROR("get WORDID error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_DEC_NO_VALID);
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(NUM,num);
	if (!r){
		num = "1";
	}

	os<<mode<<"_r"<<wordid;
	MIG_DEBUG(USER_LEVEL,"map_name:[%s]",os.str().c_str());

	os1<<"\"song\":[";
	current_num = atol(num.c_str());
	/////
	while(current_num>0){
		if (flag==0){
			flag = 1;
		}else{
			os1<<",";
		}
ret1:
		music_info = "";
	    r = storage::RedisComm::GetMusicMapRadom(os.str(),song_id);
		
		if (!r)
			//return false;
			continue;

		//是否拉黑
		r = storage::RedisComm::IsHateSong(uid,song_id);
		if (r)//拉黑
			continue;
		//是否存在
		it = songid_map.find(song_id);
		if (it!=songid_map.end())
			goto ret1;

		songid_map[song_id] = song_id;
	    r = storage::RedisComm::GetMusicInfos(song_id,music_info);

	    if (!r)
		//return false;
		    continue;

	    r = smi.UnserializedJson(music_info);
	    if (!r)
		//return false;
		    continue;

	    MIG_DEBUG(USER_LEVEL,"artist[%s] title[%s]",smi.artist().c_str(),
		    smi.title().c_str());

	    storage::DBComm::GetMusicUrl(smi.id(),hq_content_url,content_url);
	

	    smi.set_hq_url(hq_content_url);
		smi.set_url(content_url);
	    smi.set_music_time(0);
	    Base64Decode(smi.title(),&b64title);
	    Base64Decode(smi.artist(),&b64artist);
	    Base64Decode(smi.album_title(),&b64album);
		//是否是红心歌曲
		r = storage::RedisComm::IsCollectSong(uid,song_id);
		if (r)
			is_like = 1;
		else
			is_like = 0;

	    os1<<"{\"id\":\""<<smi.id().c_str()
		    <<"\",\"title\":\""<<b64title.c_str()
		    <<"\",\"artist\":\""<<b64artist.c_str()
		    <<"\",\"url\":\""<</*smi.url().c_str()*/smi.hq_url().c_str()
			<<"\",\"hqurl\":\""<<smi.hq_url().c_str()
		    <<"\",\"pub_time\":\""<<smi.pub_time().c_str()
		    <<"\",\"album\":\""<<b64album.c_str()
		    <<"\",\"time\":\""<<smi.music_time()
		    <<"\",\"pic\":\""<<smi.pic_url().c_str()
		    <<"\",\"like\":\""<<is_like<<"\"}";
		current_num--;
	}
	songid_map.clear();
	flag = 0;
	os1<<"]";
	result = os1.str();
	status = "1";
	msg = "0";
	utf8_flag = 0;
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;

}

bool MusicMgrEngine::GetOneMusicInfo(const std::string& song_id,base::MusicInfo& mi){	
	std::string content_url;
}

bool MusicMgrEngine::GetDescriptionWord(const int socket,
										const packet::HttpPacket& packet){
	packet::HttpPacket pack = packet;
	bool r = false;
	std::stringstream os;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	std::string word;
	int32 utf8_flag = 0;
	std::string b64word;
	int32 index = 0;
	std::list<base::WordAttrInfo> word_list;
	std::list<base::WordAttrInfo>::iterator it;

	r = pack.GetAttrib(DECWORD,word);
	if (!r){
		//LOG_ERROR("get DECWORD error");
		status = "1";
		msg = migfm_strerror(MIG_FM_HTTP_MOOD_DEC_NO_EXITS);
		utf8_flag = 0;
		goto ret;
	}
	if (word=="mood")
		r = storage::DBComm::GetDescriptionWord(word_list,1);
	else
		r = storage::DBComm::GetDescriptionWord(word_list,0);

	if (word_list.size()==0){
		status = "1";
		msg = migfm_strerror(MIG_FM_SYSTEM_DEC_NO_VALID);
		utf8_flag = 0;
		goto ret;
	}
	status = "1";
	msg = "0";
	os<<"\"word\":[{";
	it = word_list.begin();
	
	Base64Decode((*it).name(),&b64word);
	os<<"\"index\":\""<<index<<"\",\"typeid\":\""<<(*it).id().c_str()
		<<"\",\"name\":\""<<b64word.c_str()<<"\"}";
	word_list.pop_front();
	
	while (word_list.size()>0){
		base::WordAttrInfo dec_word = word_list.front();
		std::string b64word;
		index++;
		Base64Decode(dec_word.name(),&b64word);
		os<<",{\"index\":\""<<index<<"\",\"typeid\":\""<<dec_word.id().c_str()
			<<"\",\"name\":\""<<b64word.c_str()<<"\"}";
		word_list.pop_front();
	}

	os<<"]";

	result = os.str();
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,0);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

bool MusicMgrEngine::GetDoubanMusicChannelSong(const int socket, 
					 const packet::HttpPacket &packet){
	 packet::HttpPacket pack = packet;
	 bool r = false;
	 std::stringstream os;
	 std::string result_out;
	 std::string status;
	 std::string msg;
	 std::string result;
	 std::string channel;
	 int32 utf8_flag = 0;
	 r = pack.GetAttrib(CHANNEL,channel);
	 if (!r){
		 LOG_ERROR("get channel error");
		 status = "0";
		 msg = migfm_strerror(MIG_FM_HTTP_CHANNLE_NO_VALID);
		 utf8_flag = 0;
	 }else{
		 status = "1";
		 result = "1";
		 music_logic::MusicCacheManager* mcm = music_logic::CacheManagerOp::GetMusicCache();
		 msg = "0";
		 utf8_flag = 0;
		 mcm->IsTimeMusiChannelInfos(channel);
		 mcm->IsLessMuciChannelInfos(channel,3);
		 mcm->GetMusicChannelInfos(atol(channel.c_str()),result,1);
	 }

	 usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	 LOG_DEBUG2("[%s]",result_out.c_str());
	 usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

bool MusicMgrEngine::GetMusicChannelSong(const int socket,
										 const packet::HttpPacket& packet){
	 packet::HttpPacket pack = packet;
	 bool r = false;
	 std::stringstream os;
	 std::string result_out;
	 std::string status;
	 std::string msg;
	 std::string result;
	 std::string channel;
	 int32 utf8_flag = 0;
	 r = pack.GetAttrib(CHANNEL,channel);
	 if (!r){
		 LOG_ERROR("get channel error");
		 status = "0";
		 msg = migfm_strerror(MIG_FM_HTTP_CHANNLE_NO_VALID);
		 utf8_flag = 0;
	 }else{
		 status = "1";
		 result = "1";
		 music_logic::MusicCacheManager* mcm = music_logic::CacheManagerOp::GetMusicCache();
		 msg = "0";
		 utf8_flag = 0;
		 // mcm->IsTimeMusiChannelInfos(channel);
		 mcm->IsLessMuciChannelInfos(channel,3);
		 mcm->GetMusicChannelInfos(atol(channel.c_str()),result);
	 }

	 usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	 LOG_DEBUG2("[%s]",result_out.c_str());
	 usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

bool MusicMgrEngine::DelCollectAndHateSong(const int socket,const packet::HttpPacket& packet, 
										   const int flag){
		packet::HttpPacket pack = packet;
		bool r = false;
		int32 j = 0;
		std::string uid;
		std::string songid;
		std::string song;
		std::string result_out;
		std::string status;
		std::string msg;
		std::string result;
		std::vector<std::string> song_pair;
		int32 utf8_flag = 0;
		r = pack.GetAttrib(UID,uid);
		if (!r){
		   msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		   status = "0";
		   utf8_flag = 0;
		   goto ret;
		}
		r = pack.GetAttrib(SONGID,songid);
		if (!r){
		   msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
		   status = "0";
		   utf8_flag = 0;
		   goto ret;
		}


		
		if (base::BasicUtil::SplitStringChr(songid.c_str(),",",song_pair)==0){
			msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
			status = "0";
			utf8_flag = 0;
			goto ret;
		}

		//批量删除
		for (j=0;j<song_pair.size();j++){
			if (flag)
				storage::RedisComm::DelCollectSong(uid,song_pair[j]);
			else
				storage::RedisComm::DelHateSong(uid,song_pair[j]);
		}

		msg = "0";
		status = "1";
		utf8_flag = 0;
ret:
		usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
		LOG_DEBUG2("[%s]",result_out.c_str());
		usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
		return true;
}

bool MusicMgrEngine::GetSongList(const int socket,const packet::HttpPacket& packet,
								 const int type){
	std::list<std::string> song_list;
	packet::HttpPacket pack = packet;
	bool r = false;
	std::string content_url;
	std::string hq_content_url;
	std::string uid;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	int32 utf8_flag = 0;
	std::string songid;
	std::string mood_type;
	std::string tid;
	std::string tar_uid;
	std::string content;
	int flag = 0;
	std::string music_info;
	base::MusicInfo smi;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	std::stringstream os;
	Json::Reader reader;
	Json::Value  root;
	Json::Value songinfo;
	std::string hot_num;
	std::string clt_num;
	std::string cmt_num;
	r = pack.GetAttrib(UID,uid);
	if (!r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(TARID,tar_uid);
	if (!r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	if (type==1)//收藏歌曲表
		r = storage::RedisComm::GetCollectSongs(uid,song_list);
	else if(type==0)//默认歌曲列表
		r = storage::RedisComm::GetDefaultSongs(uid,song_list);

	if (!r){
		msg = migfm_strerror(MIG_FM_USER_NO_COLLECT_SONG);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}
	os<<"\"song\":[";
	while(song_list.size()>0){
		content = song_list.front();
		song_list.pop_front();
		if (flag==0){
			flag = 1;
		}else{
			os<<",";
		}

		//解析json
		//{"songid":"100001","type":"1","typeid":"12"}

		//收藏歌曲属性
		r = reader.parse(content.c_str(),root);
		if (!r)
			continue;
		if (root.isMember("songid"))
			songid = root["songid"].asString();
		else
			continue;

		if (root.isMember("type"))
			mood_type = root["type"].asString();
		else
			mood_type = "1";

		if (root.isMember("typeid"))
			tid = root["typeid"].asString();
		else
			tid = "1";

		r = storage::RedisComm::GetMusicInfos(songid,music_info);
		if (!r)
			continue;

		r = smi.UnserializedJson(music_info);
		if (!r)
			continue;

		//获取收藏数//获取评论数//热度
		GetMusicHotCltCmt(songid,hot_num,cmt_num,clt_num);
		smi.set_music_clt(clt_num);
		smi.set_music_cmt(cmt_num);
		smi.set_music_hot(hot_num);

		storage::DBComm::GetMusicUrl(smi.id(),hq_content_url,content_url);
		smi.set_hq_url(hq_content_url);
		smi.set_url(content_url);
		smi.set_music_time(0);
		Base64Decode(smi.title(),&b64title);
		Base64Decode(smi.artist(),&b64artist);
		Base64Decode(smi.album_title(),&b64album);
		os<<"{\"id\":\""<<smi.id().c_str()
			<<"\",\"title\":\""<<b64title.c_str()
			<<"\",\"artist\":\""<<b64artist.c_str()
			<<"\",\"url\":\""<<smi.url().c_str()
			<<"\",\"hqurl\":\""<<smi.hq_url().c_str()
			<<"\",\"pub_time\":\""<<smi.pub_time().c_str()
			<<"\",\"album\":\""<<b64album.c_str()
			<<"\",\"time\":\""<<smi.music_time()
			<<"\",\"pic\":\""<<smi.pic_url().c_str()
			<<"\",\"type\":\""<<mood_type.c_str()
			<<"\",\"typeid\":\""<<tid.c_str()
			<<""
			<<"\",\"like\":\"0\"}";
	}
	flag = 0;
	os<<"]";
	result = os.str();
	status = "1";
	msg = "0";
	utf8_flag = 0;
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;

}

bool MusicMgrEngine::GetTypeSongs(const int socket,const packet::HttpPacket& packet){
	packet::HttpPacket pack = packet;
	bool r = false;
	std::stringstream os;
	std::stringstream os1;
	std::string uid;
	std::string mood_id;
	std::string mood_index;
	std::string scene_id;
	std::string scens_index;
	std::string channel_id;
	std::string channel_index;
	std::string num;
	std::string result_out;
	std::string result;
	std::string status;
	std::string msg;
	int32 utf8_flag = 0;
	std::string mode;
	int32 temp_index = 0;
	base::CompareInfo moodci;
	base::CompareInfo scensci;
	base::CompareInfo channelci;
	int32 mood_num;
	int32 scens_num;
	int32 channel_num;
	int32 mood_flag;
	int32 scens_flag;
	int32 channel_flag;
	int32 flag_al = 0;
	music_logic::MusicCacheManager* mcm = music_logic::CacheManagerOp::GetMusicCache();

	r = pack.GetAttrib(MOODINDEX,mood_index);
	if (!r){
		msg = migfm_strerror(MIG_FM_MOODINDEX_NO_VALID);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}


	r = pack.GetAttrib(MOODID,mood_id);
	if (!r){
		if (mood_index!="-1"){
			msg = migfm_strerror(MIG_FM_MOODID_NO_VALID);
			status = "0";
			utf8_flag = 0;
			goto ret;
		}
	}

	r = pack.GetAttrib(SCENEINDEX,scens_index);
	if (!r){
		msg = migfm_strerror(MIG_FM_SCENEINDEX_NO_VALID);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(SCENEID,scene_id);
	if (!r){
		if (scens_index!="-1"){
			msg = migfm_strerror(MIG_FM_SCENEID_NO_VALID);
			status = "0";
			utf8_flag = 0;
			goto ret;
		}
	}

	r = pack.GetAttrib(TYPEINDEX,channel_index);
	if (!r){
		msg = migfm_strerror(MIG_FM_CHANNELINDEX_NO_VALID);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(TYPEID,channel_id);
	if (!r){
		if (channel_index!="-1"){
			msg = migfm_strerror(MIG_FM_CHANNELID_NO_VALID);
			status = "0";
			utf8_flag = 0;
			goto ret;
		}
	}


	r = pack.GetAttrib(NUM,num);
	//fix me
	if (!r)//若没提交个数 默认10首
		num = "10";

	//判断模式是否为-1 单选模式
	if ((atol(mood_index.c_str())!=-1)&&(atol(scens_index.c_str())!=-1)&&(atol(channel_index.c_str())!=-1)){
		flag_al = 1;
	}

	if (atoi(mood_index.c_str())<0){
		mood_flag = 0;
		mood_id = "0";
		mood_index = "0";
	}
	else
		mood_flag = 1;

	if (atoi(scens_index.c_str())<0){
		scens_flag = 0;
		scene_id = "0";
		scens_index = "0";
	}
	else
		scens_flag = 1;

	if (atoi(channel_index.c_str())<0){
		channel_flag = 0;
		channel_id = "0";
		channel_index = "0";
	}
	else
		channel_flag = 1;



	//判断提交序号三个不能同时为0
	if((mood_index=="0")&&(scens_index=="0")&&(channel_index=="0"))
		mood_index = scens_index = channel_index ="1";

	//判断提交个数不能为0
	if (num=="0")
		num = "10";

	moodci.set_info_id(mood_id);
	moodci.set_info_index(mood_index);
	scensci.set_info_id(scene_id);
	scensci.set_info_index(scens_index);
	channelci.set_info_id(channel_id);
	channelci.set_info_index(channel_index);

	os1<<"\"song\":[";
	if (flag_al){
			algorithm::AlgorithmBase::AllocationLatitudeMusicNum(atol(mood_index.c_str()),mood_num,
			atol(scens_index.c_str()),scens_num,
			atol(channel_index.c_str()),channel_num,
			atol(num.c_str()));

			moodci.set_info_num(mood_num);
			scensci.set_info_num(scens_num);
			channelci.set_info_num(channel_num);
	}else{//单一模式
		if(atol(mood_id.c_str())!=0)
			moodci.set_info_num(atol(num.c_str()));
		else if (atol(scene_id.c_str())!=0)
			scensci.set_info_num(atol(num.c_str()));
		else if (atol(channel_id.c_str())!=0)
			channelci.set_info_num(atol(num.c_str()));
	}


	if (mood_flag){
		mode = "mm";
		r = GetMoodScensChannelSongs(uid,mode,moodci.info_num(),moodci.info_id(),os1);//心情
		if (r){
			if (scens_flag!=0||channel_flag!=0)
				os1<<",";
		}
	}


	if (scens_flag){
		mode = "ms";
		r = GetMoodScensChannelSongs(uid,mode,scensci.info_num(),scensci.info_id(),os1);//场景
		if (r){
			if (mood_flag!=0||channel_flag!=0)
				os1<<",";
		}
		LOG_DEBUG2("OS1[%s]",os1.str().c_str());
	}

	if (channel_flag){
		mode = "chl";
		r = GetMoodScensChannelSongs(uid,mode,channelci.info_num(),channelci.info_id(),os1);//频道
	}

	if (!r){
		result = os1.str().erase(os1.str().length()-1,1);
	}else{
		result = os1.str();
	}
	//os1<<"]";
	result.append("]");

	msg = "0";
	utf8_flag = 0;
	status = "1";
	

	//result = os1.str();
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
}

bool MusicMgrEngine::GetUserMusicCltAndHis(const int socket,
										   const packet::HttpPacket& packet){
	packet::HttpPacket pack = packet;
	std::string uid;
	std::string from_id;
	std::string count;
	std::string is_like;
	std::string status;
	std::string msg;
	std::string result;
	std::string result_out;
	std::list<std::string> songlist;
	std::list<std::string> collect_list;
	std::list<std::string> history_list;
	Json::FastWriter wr;
	Json::Value value;

	if (!pack.GetAttrib(UID,uid)){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
	}
	
	if (!pack.GetAttrib(FROMID,from_id))
		from_id = "0";

	if (!pack.GetAttrib(COUNT,count))
		count = "10";
	
	if (!pack.GetAttrib(ISLIKE,is_like))
		is_like = "0";

	//获取历史歌曲
	storage::DBComm::GetUserHistoryMusic(uid,from_id,count,songlist);

	storage::RedisComm::GetMusicHistroyCollect(uid,is_like,songlist,
		collect_list,history_list);
	if (history_list.size()>0){
		Json::Value& history = value["history"];
		while (history_list.size()>0){
			Json::Value music;
			std::string musicinfo;
			base::MusicInfo smi;
			std::string b64title;
			std::string b64artist;
			std::string b64album;
			musicinfo = history_list.front();
			history_list.pop_front();
			smi.UnserializedJson(musicinfo);
			Base64Decode(smi.title(),&b64title);
			Base64Decode(smi.artist(),&b64artist);
			Base64Decode(smi.album_title(),&b64album);
			music["id"] = smi.id();
			music["title"] = b64title;
			music["artist"] = b64artist;
			music["pic"] = smi.pic_url();
			music["album"] = b64album;
			history.append(music);
		}
	}

	if(collect_list.size()>0){
		Json::Value& collect = value["collect"];
		while (collect_list.size()>0){
			Json::Value music;
			std::string musicinfo;
			base::MusicInfo smi;
			std::string b64title;
			std::string b64artist;
			std::string b64album;
			musicinfo = collect_list.front();
			collect_list.pop_front();
			smi.UnserializedJson(musicinfo);
			Base64Decode(smi.title(),&b64title);
			Base64Decode(smi.artist(),&b64artist);
			Base64Decode(smi.album_title(),&b64album);
			music["id"] = smi.id();
			music["title"] = b64title;
			music["artist"] = b64artist;
			music["pic"] = smi.pic_url();
			music["album"] = b64album;
			collect.append(music);
		}
	}
	status = "1";
ret:
	value["status"] = status;
	value["msg"] = msg;
	result_out = wr.write(value);
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
}

bool MusicMgrEngine::SetMoodRecording(const int socket,
									  const packet::HttpPacket& packet){
	packet::HttpPacket pack = packet;
	bool r = false;
	std::string uid;
	std::string mode;
	std::string wordid;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	std::string songid;
	std::string lastsongid;
	std::string name;
	std::string singer;
	std::string state;
	int32 utf8_flag = 0;
	int32 recording_flag = 1;

	r = pack.GetAttrib(UID,uid);
	if (!r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 0;
		recording_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(MODE,mode);
	if (!r){
		LOG_ERROR("get MOOD error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_DEC_NO_VALID);
		utf8_flag = 0;
		recording_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(TYPEID,wordid);
	if (!r){
		LOG_ERROR("get MOOD error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_DEC_NO_VALID);
		utf8_flag = 0;
		recording_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(UID,uid);
	if (!r){
		LOG_ERROR("get uid error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		utf8_flag = 0;
		recording_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(CURSONG,songid);
	if (!r){
		LOG_ERROR("get songid error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
		utf8_flag = 0;
		recording_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(LASTSONG,lastsongid);
	if (!r){
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
		utf8_flag = 0;
		recording_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(NAME,name);
	if (!r)
		name = "未知歌曲";

	r = pack.GetAttrib(SINGER,singer);
	if (!r)
		singer = "佚名";

	r = pack.GetAttrib(STATE,state);
	if(!r){
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
		utf8_flag = 0;
		recording_flag = 0;
		goto ret;
	}

	msg = "0";
	status = "1";
	utf8_flag = 0;
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	//recording mood and current songid
	if (recording_flag){
		//只记录心情
		if(mode=="mm")
		   mood_record_engine_->RecordingMood(atoll(uid.c_str()),
		                                   atol(wordid.c_str()));
		//value {"songid":"10000","state":"1","type":"mm","tid":"1","name":"艳阳天","singer":"窦唯"}

		storage::MemComm::SetUsrCurrentSong(uid,songid,name,singer,state,mode,wordid);
		storage::DBComm::RecordMusicHistory(uid,songid);
		//记录用户听歌历史
		if (state!="2"){//本地手机歌曲不记录
			storage::RedisComm::MgrListenSongsNum(songid,lastsongid,uid);
			//storage::RedisComm::MgrListenSongsNum(songid,uid,1);
			//if (atol(lastsongid.c_str())!=0)
				//storage::RedisComm::MgrListenSongsNum(lastsongid,uid,0);
		}
		if (songid!="0")
			//记录热度
			SetMusicHostCltCmt(songid,1);
	}
	return true;

}

bool MusicMgrEngine::PostCollectAndHateSong(const int socket,
											const packet::HttpPacket& packet,
											const int flag){
	packet::HttpPacket pack = packet;
	bool r = false;
	std::string uid;
	std::string songid;
	std::string moodtype;
	std::string tid;
	std::stringstream os;
	std::stringstream collect;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	std::string content;
	int32 utf8_flag = 0;
	r = pack.GetAttrib(UID,uid);
	if (!r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}
	r = pack.GetAttrib(SONGID,songid);
	if (!r){
		msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	if (flag){
		r = pack.GetAttrib(MODETYPE,moodtype);
		if (!r){
			moodtype = "1";
			/*msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
			status = "0";
			utf8_flag = 0;
			goto ret;*/
		}

		r = pack.GetAttrib(TYPEID,tid);
		if (!r){
			tid = "1";
			/*msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
			status = "0";
			utf8_flag = 0;
			goto ret;*/
		}

		//{"songid":"100001","type":"1","typeid":"12"}

		os<<"{\"songid\":\""<<songid.c_str()
			<<"\",\"type\":\""<<moodtype.c_str()
			<<"\",\"typeid\":\""<<tid.c_str()
			<<"\"}";
	}

	content = os.str();
	if (flag)
		storage::RedisComm::SetCollectSong(uid,songid,content);
	else
		storage::RedisComm::SetHateSong(uid,songid);

	//记录收藏数
	SetMusicHostCltCmt(songid,2);
	msg = "0";
	status = "1";
	utf8_flag = 0;
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
}



bool MusicMgrEngine::GetMusicChannel(const int socket,
  									 const packet::HttpPacket& packet){
	 packet::HttpPacket pack = packet;
	 bool r = false;
	 std::string num;
	 std::stringstream os;
	 std::string result_out = "0";
	 std::string status = "1";
	 std::string msg = "0";
	 std::string result;
	 int32 utf8_flag = 0;
	 music_logic::MusicCacheManager* mcm = music_logic::CacheManagerOp::GetMusicCache();
	 assert(mcm);
	 r = pack.GetAttrib(CHANNELNUM,num);
	 if (!r){
		 LOG_ERROR("get channel error");
		 msg = migfm_strerror(MIG_FM_HTTP_CHANNLE_NO_VALID);
		 status = "0";
		 utf8_flag = 0;
	 }else{
		 mcm->GetMusicChannel(num,result);
		 LOG_DEBUG2("[%s]",result.c_str());
		 utf8_flag = 0;
	 }
 	 usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
 	 LOG_DEBUG2("[%s]",result_out.c_str());
 	 usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	 return true;
}

bool MusicMgrEngine::GetMusicInfos(const int socket,const std::string& songid){
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
	base::MusicInfo smi;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	bool r = false;

	r = storage::RedisComm::GetMusicInfos(songid,music_info);

	if (!r)
		return false;

	r = smi.UnserializedJson(music_info);
	if (!r)
		return false;
	MIG_DEBUG(USER_LEVEL,"artist[%s] title[%s]",smi.artist().c_str(),
		smi.title().c_str());

	storage::DBComm::GetWXMusicUrl(smi.id(),content_url,dec,dec_id,dec_word);

	smi.set_hq_url(content_url);
	smi.set_url(content_url);
	Base64Decode(smi.title(),&b64title);
	Base64Decode(smi.artist(),&b64artist);
	Base64Decode(smi.album_title(),&b64album);
	os1<<"\"song\":[{";
	os1<<"\"id\":\""<<smi.id().c_str()
		<<"\",\"title\":\""<<b64title.c_str()
		<<"\",\"artist\":\""<<b64artist.c_str()
		<<"\",\"url\":\""<<smi.url().c_str()
		<<"\",\"hqurl\":\""<<smi.hq_url().c_str()
		<<"\",\"pub_time\":\""<<smi.pub_time().c_str()
		<<"\",\"album\":\""<<b64album.c_str()
		<<"\",\"pic\":\""<<smi.pic_url().c_str()
		<<"\",\"like\":\"0\"}]";
	    os1<<",\"mode\":\""<<dec.c_str()
		<<"\",\"wordid\":\""<<dec_id.c_str()<<"\",\"name\":\""
		<<dec_word.c_str()<<"\"";
	result = os1.str();
	status = "1";
	msg = "0";
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,0);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	return true;
}

bool MusicMgrEngine::GetMoodScensChannelSongs(const std::string& uid,
									   const std::string mode, 
									   const int32 num, const std::string wordid,
									   std::stringstream& result){
	std::stringstream os;
	bool r = false;
	os<<mode.c_str()<<"_r"<<wordid.c_str();
	int32 temp_index = 0;
	int32 temp_total = 0;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	int is_like = 0;
	std::map<std::string, std::string> songid_map;
	std::map<std::string, std::string>::iterator it;
	//先获取整个hash 值
	int hash_size = storage::RedisComm::GetHashSize(os.str());
	if (hash_size==0)
		return false;
	temp_index = num>hash_size?hash_size:num;
	temp_total = algorithm::AlgorithmBase::GetTotalForNum(temp_index,3);
	while(temp_index>0&&temp_total>0){
		std::string songid;
		std::string music_info;
		std::string hq_content_url;
		std::string content_url;
		std::string hot_num;
		std::string clt_num;
		std::string cmt_num;
		base::MusicInfo smi;
		r = storage::RedisComm::GetMusicMapRadom(os.str(),songid);
		if (!r){
			temp_total--;
			continue;
		}
		//是否拉黑
		r = storage::RedisComm::IsHateSong(uid,songid);
		if (r){
			temp_total--;
			continue;
		}
		//是否已经存在
		it = songid_map.find(songid);
		if (it!=songid_map.end()){
			temp_total--;
			continue;
		}
		songid_map[songid] = songid;
		r = storage::RedisComm::GetMusicInfos(songid,music_info);
		if (!r){
			temp_total--;
			continue;
		}
		r =smi.UnserializedJson(music_info);
		if (!r){
			temp_total--;
			continue;
		}
		storage::DBComm::GetMusicUrl(smi.id(),hq_content_url,content_url);
		smi.set_hq_url(hq_content_url);
		smi.set_url(content_url);
		smi.set_music_time(0);
		Base64Decode(smi.title(),&b64title);
		Base64Decode(smi.artist(),&b64artist);
		Base64Decode(smi.album_title(),&b64album);
		//是否是红心歌曲
		r = storage::RedisComm::IsCollectSong(uid,songid);
		if (r)
			is_like = 1;
		else
			is_like = 0;

		//获取收藏数//获取评论数//热度
		GetMusicHotCltCmt(songid,hot_num,cmt_num,clt_num);
		smi.set_music_clt(clt_num);
		smi.set_music_cmt(cmt_num);
		smi.set_music_hot(hot_num);
		
		result<<"{\"id\":\""<<smi.id().c_str()
			<<"\",\"title\":\""<<b64title.c_str()
			<<"\",\"artist\":\""<<b64artist.c_str()
			<<"\",\"url\":\""<<smi.url().c_str()
			<<"\",\"hqurl\":\""<<smi.hq_url().c_str()
			<<"\",\"pub_time\":\""<<smi.pub_time().c_str()
			<<"\",\"album\":\""<<b64album.c_str()
			<<"\",\"time\":\""<<smi.music_time()
			<<"\",\"pic\":\""<<smi.pic_url().c_str()
			<<"\",\"type\":\""<<mode.c_str()
			<<"\",\"tid\":\""<<wordid.c_str()
			<<"\",\"clt\":\""<<smi.clt_num().c_str()
			<<"\",\"cmt\":\""<<smi.cmt_num().c_str()
			<<"\",\"hot\":\""<<smi.hot_num().c_str()
			<<"\",\"like\":\""<<is_like<<"\"}";
		if (temp_index!=1)
			result<<",";

		temp_index--;
		temp_total--;
	}
	return true;
}

bool MusicMgrEngine:: UpdateConfigFile(const int socket,
									   const packet::HttpPacket& packet){
	packet::HttpPacket pack = packet;
	std::string version;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	std::string content;
	std::string key;
	int32 utf8_flag = 0;
	bool r = false;
	Json::Reader reader;
	Json::Value  root;
	Json::Value music;
	std::string json_version;

	r = pack.GetAttrib(VERSION,version);
	if (!r){
		msg = migfm_strerror(MIG_FM_VERSION_NO_EXITS);
		status = "0";
		utf8_flag =1;
		goto ret;
	}
	//key clic
	key = "clic";
	r = storage::RedisComm::GetUpdateConfig(key,content);
	if (!r){
		msg = migfm_strerror(MIG_FM_VERSION_NO_EXITS);
		status = "0";
		utf8_flag =1;
		goto ret;
	}

	LOG_DEBUG2("[%s]",content.c_str());
	content = "{\"filename\":\"channelinfo.xml\",\"version\":\"1.0.0\",\"url\":\"http://www.baidu.com/migfm\"}";
	//解析json
	r = reader.parse(content.c_str(),root);
	if (!r){
		LOG_ERROR("parser json error");
		return false;
	}
	LOG_DEBUG2("%s",content.c_str());
	if (root.isMember("version")){
		json_version = root["version"].asString();
	}
	if (json_version!=version){
		msg = "0";
		status = "1";
		result = content;
		goto ret;
	}
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

bool MusicMgrEngine::PostUserLocalMusicinfos(const int socket,
											 const packet::HttpPacket& packet){

	packet::HttpPacket pack = packet;
	std::string source;
	std::string urlcode;
	std::string name;
	std::string uid;
	std::string content;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	int32 utf8_flag = 0;
	bool r = false;
	int32 music_infos_size;
	Json::Reader reader;
	Json::Value  root;
	Json::Value music;
	std::list<base::RecordingLocalMusic> rlm_list;
	r = pack.GetAttrib(UID,uid);
	if (!r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}
	
	r = pack.GetAttrib(SOURCE,source);
	if (!r){
		msg = migfm_strerror(MIG_FM_SOUCE_NO_VALID);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(URLCODE,urlcode);//默认不进行urlcode 编码

	r = pack.GetAttrib(NAME,name);
	if (!r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	r = pack.GetAttrib(CONTENT,content);
	if (!r){
		msg = migfm_strerror(MIG_FM_CONTENT_NO_EXITS);
		status = "0";
		utf8_flag = 0;
		goto ret;
	}

	r = reader.parse(content.c_str(),root);
	if (!r){
		LOG_ERROR("parser json error");
		return false;
	}
	music_infos_size = root["music"].size();
	if (music_infos_size<=0){
		LOG_ERROR2("song valiled size[%d]",music_infos_size);
		return false;
	}
	music = root["music"];
	for (int i =0;i<music_infos_size;i++){
		std::string name;
		std::string song;
		if (music[i].isMember("name")){
			name = music[i]["name"].asString();
		}

		if (music[i].isMember("singer")){
			song = music[i]["singer"].asString();
		}
		base::RecordingLocalMusic rlm(name,song);
		rlm_list.push_back(rlm);
	}
	msg = "0";
	status = "1";
	utf8_flag = 0;
ret:
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
	LOG_DEBUG2("[%s]",result_out.c_str());
	usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	if (rlm_list.size()>0)
		user_local_music_engine_->RecordingLocalMusic(uid,source,rlm_list);
	return true;
}

bool MusicMgrEngine::SetMusicHostCltCmt(const std::string& songid,const int32 flag)
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

	storage::RedisComm::SetMusicAboutUser(songid,hot_num,cmt_num,clt_num);
}


bool MusicMgrEngine::GetMusicHotCltCmt(const std::string &songid, 
									   std::string &hot_num, 
									   std::string &cmt_num, 
									   std::string &clt_num){
	std::string content;
	bool r = false;
	Json::Reader reader;
	Json::Value  root;
	Json::Value songinfo;
	r = storage::RedisComm::GetMusicAboutUser(songid,content);
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


}