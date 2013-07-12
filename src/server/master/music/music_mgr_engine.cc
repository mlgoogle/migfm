#include "music_mgr_engine.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "logic_comm.h"
#include "music_cache_mgr.h"
#include "basic/constants.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include "basic/base64.h"
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

	std::string name = "hello world";
	std::string b64_name;
	std::string deb64_name;

	Base64Encode(name,&b64_name);
	LOG_DEBUG2("b64_name[%s]",b64_name.c_str());

	Base64Decode(b64_name,&deb64_name);
	LOG_DEBUG2("deb64_name[%s]",deb64_name.c_str());

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
	}

    return true;
}


bool MusicMgrEngine::GetMoodSceneWordSong(const int socket,
										  const packet::HttpPacket& packet){
	
	packet::HttpPacket pack = packet;
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
	base::MusicInfo smi;
	std::string b64title;
	std::string b64artist;
	std::string b64album;
	bool r = pack.GetAttrib(MODE,mode);
	if (!r){
		LOG_ERROR("get MODE error");
		return false;
	}

	r = pack.GetAttrib(WORDID,wordid);
	if (!r){
		LOG_ERROR("get WORDID error");
		return false;
	}

	os<<mode<<"_r"<<wordid;
	std::string song_id;
	MIG_DEBUG(USER_LEVEL,"map_name:[%s]",os.str().c_str());
	r = storage::RedisComm::GetMusicMapRadom(os.str(),song_id);

	if (!r)
		return false;

	r = storage::RedisComm::GetMusicInfos(song_id,music_info);

	if (!r)
		return false;

	r = smi.UnserializedJson(music_info);
	if (!r)
		return false;
	MIG_DEBUG(USER_LEVEL,"artist[%s] title[%s]",smi.artist().c_str(),
		smi.title().c_str());

	r = get_song_engine_->GetSongInfo(smi.artist(),smi.title(),
		                          smi.album_title(),content_url);
	if (!r)
		return true;

	smi.set_url(content_url);
	Base64Decode(smi.title(),&b64title);
	Base64Decode(smi.artist(),&b64artist);
	Base64Decode(smi.album_title(),&b64album);
	os1<<"\"word\":[{";
	os1<<"\"id\":\""<<smi.id().c_str()
		<<"\",\"title\":\""<<b64title.c_str()
		<<"\",\"artist\":\""<<b64artist.c_str()
		<<"\",\"url\":\""<<smi.url().c_str()
		<<"\",\"pub_time\":\""<<smi.pub_time().c_str()
		<<"\",\"album\":\""<<b64album.c_str()
		<<"\",\"pic\":\""<<smi.pic_url().c_str()
		<<"\",\"like\":\"0\"}]";
	result = os1.str();
	status = "0";
	msg = "0";
	usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,0);
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
	std::list<base::WordAttrInfo> word_list;

	r = pack.GetAttrib(DECWORD,word);
	if (!r){
		LOG_ERROR("get DECWORD error");
		return false;
	}
	if (word=="mood")
		r = storage::DBComm::GetDescriptionWord(word_list,1);
	else
		r = storage::DBComm::GetDescriptionWord(word_list,0);

	if (word_list.size()==0)
		return true;
	status = "0";
	msg = "0";
	os<<"\"word\":[{";
	std::list<base::WordAttrInfo>::iterator it = word_list.begin();
	os<<"\"typeid\":\""<<(*it).id().c_str()
		<<"\",\"name\":\""<<(*it).name().c_str()<<"\"}";
	word_list.pop_front();
  
	while (word_list.size()>0){
		base::WordAttrInfo dec_word = word_list.front();
		os<<",{\"typeid\":\""<<dec_word.id().c_str()
			<<"\",\"name\":\""<<dec_word.name().c_str()<<"\"}";
		word_list.pop_front();
	}

	os<<"]";

	result = os.str();
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
	 r = pack.GetAttrib(CHANNEL,channel);
	 if (!r){
		 LOG_ERROR("get channel error");
		 return false;
	 }
	 status = "1";
	 result = "1";
	 music_logic::MusicCacheManager* mcm = music_logic::CacheManagerOp::GetMusicCache();
	 //
	 status = "0";
	 msg = "0";
	 mcm->IsTimeMusiChannelInfos(channel);
	 mcm->IsLessMuciChannelInfos(channel,3);
	 mcm->GetMusicChannelInfos(atol(channel.c_str()),result);
	 usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,0);
	 LOG_DEBUG2("[%s]",result_out.c_str());
	 usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
}

bool MusicMgrEngine::GetMusicChannel(const int socket,
  									 const packet::HttpPacket& packet){
	 packet::HttpPacket pack = packet;
	 bool r = false;
	 std::string num;
	 std::stringstream os;
	 std::string result_out = "0";
	 std::string status = "0";
	 std::string msg = "0";
	 std::string result;
	 music_logic::MusicCacheManager* mcm = music_logic::CacheManagerOp::GetMusicCache();
	 assert(mcm);
	 r = pack.GetAttrib(CHANNELNUM,num);
	 if (!r){
		 LOG_ERROR("get channel error");
		 return false;
	 }
	 mcm->GetMusicChannel(num,result);
	 LOG_DEBUG2("[%s]",result.c_str());
 	 usr_logic::SomeUtils::GetResultMsg(status,msg,result,result_out,0);
 	 LOG_DEBUG2("[%s]",result_out.c_str());
 	 usr_logic::SomeUtils::SendFull(socket,result_out.c_str(),result_out.length());
	 return true;
}





}