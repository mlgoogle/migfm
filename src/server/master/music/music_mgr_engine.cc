#include "music_mgr_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "music_cache_mgr.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include "basic/base64.h"
#include "basic/errno_comm.h"
#include <sstream>

#define		TIME_TEST		1025

namespace music_logic{


MusicMgrEngine::MusicMgrEngine(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	usr_logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return;
	}
	r = config->LoadConfig(path);
	storage::DBComm::Init(config->mysql_db_list_);
	storage::MemComm::Init(config->mem_list_);
	storage::RedisComm::Init(config->redis_list_);
	music_logic::CacheManagerOp::GetMusicCache();

	get_song_engine_ = music_logic::GetSongUrl::Create(music_logic::TYPE_SOGOU);
	if (get_song_engine_==NULL){
		MIG_ERROR(USER_LEVEL,"get_song create error");
		return ;
	}
	std::string get_song_url = "http://121.199.32.88/getmusicurl.ashx";
	get_song_engine_->Init(get_song_url);

}

MusicMgrEngine::~MusicMgrEngine(){

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

bool MusicMgrEngine::OnMusicMgrClose(struct server *srv, int socket){
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
	}else if (type=="setcltsong"){
		PostCollectAndHateSong(socket,packet,1);
	}else if (type=="getcltsonglist"){
		GetCllectSongList(socket,packet);
	}else if (type=="delcltsong"){
		DelCollectAndHateSong(socket,packet,1);
	}else if (type=="sethtsong"){
		PostCollectAndHateSong(socket,packet,0);
	}else if (type=="delthsong"){
		DelCollectAndHateSong(socket,packet,0);
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
		utf8_flag = 1;
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
		utf8_flag = 1;
		goto ret;
	}

	//从redis 里面获取用户心绪图
	//音乐机器人专门从操作记录里面分析
	r = storage::RedisComm::GetUserMoodMap(uid,result);
	if (result.empty()){
		//LOG_ERROR("Get UID Error");
		status = "1";
		msg = migfm_strerror(MIG_FM_USER_MOOD_NO_EXITS);
		utf8_flag = 1;
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
	std::string num;
	base::MusicInfo smi;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	std::string song_id;
	int current_num;
	int flag = 0;
	std::map<std::string, std::string> songid_map;
	std::map<std::string, std::string>::iterator it;
	bool r = pack.GetAttrib(MODE,mode);
	if (!r){
		//LOG_ERROR("get MODE error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_MODE_NO_VALID);
		utf8_flag = 1;
		goto ret;
	}

	r = pack.GetAttrib(WORDID,wordid);
	if (!r){
		LOG_ERROR("get WORDID error");
		status = "0";
		msg = migfm_strerror(MIG_FM_HTTP_DEC_NO_VALID);
		utf8_flag = 1;
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
	    r = storage::RedisComm::GetMusicMapRadom(os.str(),song_id);
		
		if (!r)
			//return false;
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

	    storage::DBComm::GetMusicUrl(smi.id(),content_url);
	

	    smi.set_url(content_url);
	    smi.set_music_time(0);
	    Base64Decode(smi.title(),&b64title);
	    Base64Decode(smi.artist(),&b64artist);
	    Base64Decode(smi.album_title(),&b64album);
	    os1<<"{\"id\":\""<<smi.id().c_str()
		    <<"\",\"title\":\""<<b64title.c_str()
		    <<"\",\"artist\":\""<<b64artist.c_str()
		    <<"\",\"url\":\""<<smi.url().c_str()
		    <<"\",\"pub_time\":\""<<smi.pub_time().c_str()
		    <<"\",\"album\":\""<<b64album.c_str()
		    <<"\",\"time\":\""<<smi.music_time()
		    <<"\",\"pic\":\""<<smi.pic_url().c_str()
		    <<"\",\"like\":\"0\"}";
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
		utf8_flag = 1;
		goto ret;
	}
	if (word=="mood")
		r = storage::DBComm::GetDescriptionWord(word_list,1);
	else
		r = storage::DBComm::GetDescriptionWord(word_list,0);

	if (word_list.size()==0){
		status = "1";
		msg = migfm_strerror(MIG_FM_SYSTEM_DEC_NO_VALID);
		utf8_flag = 1;
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
		 utf8_flag = 1;
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
		std::string uid;
		std::string songid;
		std::string result_out;
		std::string status;
		std::string msg;
		std::string result;
		int32 utf8_flag = 0;
		r = pack.GetAttrib(UID,uid);
		if (r){
		   msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		   status = "0";
		   utf8_flag = 1;
		   goto ret;
		}
		r = pack.GetAttrib(SONGID,songid);
		if (r){
		   msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
		   status = "0";
		   utf8_flag = 1;
		   goto ret;
		}

		if (flag)
		   storage::RedisComm::DelCollectSong(uid,songid);
		else
		   storage::RedisComm::DelHateSong(uid,songid);

		msg = "0";
		status = "1";
		utf8_flag = 0;
ret:
		usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,utf8_flag);
		LOG_DEBUG2("[%s]",result_out.c_str());
		usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
		return true;
}

bool MusicMgrEngine::GetCllectSongList(const int socket,const packet::HttpPacket& packet){
	std::list<std::string> song_list;
	packet::HttpPacket pack = packet;
	bool r = false;
	std::string content_url;
	std::string uid;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	int32 utf8_flag = 0;
	std::string songid;
	int flag = 0;
	std::string music_info;
	base::MusicInfo smi;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	std::stringstream os;
	r = pack.GetAttrib(UID,uid);
	if (r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 1;
		goto ret;
	}

	r = storage::RedisComm::GetCollectSongs(uid,song_list);
	if (r){
		msg = migfm_strerror(MIG_FM_USER_NO_COLLECT_SONG);
		status = "0";
		utf8_flag = 1;
		goto ret;
	}
	os<<"\"song\":[";
	while(song_list.size()>0){
		songid = song_list.front();
		song_list.pop_front();
		if (flag==0){
			flag = 1;
		}else{
			os<<",";
		}

		r = storage::RedisComm::GetMusicInfos(songid,music_info);

		if (!r)
			continue;

		r = smi.UnserializedJson(music_info);
		if (!r)
			continue;

		storage::DBComm::GetMusicUrl(smi.id(),content_url);
		smi.set_url(content_url);
		smi.set_music_time(0);
		Base64Decode(smi.title(),&b64title);
		Base64Decode(smi.artist(),&b64artist);
		Base64Decode(smi.album_title(),&b64album);
		os<<"{\"id\":\""<<smi.id().c_str()
			<<"\",\"title\":\""<<b64title.c_str()
			<<"\",\"artist\":\""<<b64artist.c_str()
			<<"\",\"url\":\""<<smi.url().c_str()
			<<"\",\"pub_time\":\""<<smi.pub_time().c_str()
			<<"\",\"album\":\""<<b64album.c_str()
			<<"\",\"time\":\""<<smi.music_time()
			<<"\",\"pic\":\""<<smi.pic_url().c_str()
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

bool MusicMgrEngine::PostCollectAndHateSong(const int socket,const packet::HttpPacket& packet,
											const int flag){
	packet::HttpPacket pack = packet;
	bool r = false;
	std::string uid;
	std::string songid;
	std::string result_out;
	std::string status;
	std::string msg;
	std::string result;
	int32 utf8_flag = 0;
	r = pack.GetAttrib(UID,uid);
	if (r){
		msg = migfm_strerror(MIG_FM_HTTP_USER_NO_EXITS);
		status = "0";
		utf8_flag = 1;
		goto ret;
	}
	r = pack.GetAttrib(SONGID,songid);
	if (r){
		msg = migfm_strerror(MIG_FM_HTTP_SONG_ID_NO_VALID);
		status = "0";
		utf8_flag = 1;
		goto ret;
	}

	if (flag)
		storage::RedisComm::SetCollectSong(uid,songid);
	else
		storage::RedisComm::SetHateSong(uid,songid);

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
		 utf8_flag = 1;
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

	smi.set_url(content_url);
	Base64Decode(smi.title(),&b64title);
	Base64Decode(smi.artist(),&b64artist);
	Base64Decode(smi.album_title(),&b64album);
	os1<<"\"song\":[{";
	os1<<"\"id\":\""<<smi.id().c_str()
		<<"\",\"title\":\""<<b64title.c_str()
		<<"\",\"artist\":\""<<b64artist.c_str()
		<<"\",\"url\":\""<<smi.url().c_str()
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



}