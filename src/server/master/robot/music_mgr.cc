#include "music_mgr.h"
#include "db_comm.h"
#include "dic_comm.h"
#include <list>
#include <string>

namespace robot_logic{

MusicMgr::MusicMgr()
:from_(0)
,count_(10){

}

MusicMgr::~MusicMgr(){

}

//getvailedlyric.fcgi
bool MusicMgr::GetVailedLyric(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
    Json::Value& content = result["result"];
    std::list<int64> songid_list;
	packet::HttpPacket pack = packet;
	r = robot_storage::DBComm::GetVailedLyric(songid_list,from_,count_);
	if(!r){
	    result["status"] = 0;
	    result["msg"] = "无歌词歌曲";
	    content = "1";
	    goto ret;
	    from_ = 0;
	}
	from_+=songid_list.size();

	GetBatchMusicInfo(songid_list,content);
    result["status"] = 1;
    result["msg"] = "";
ret:
	reponse = wr.write(result);
    r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
   		 reponse.length());
    return r;
}

//getvailedurlmusic.fcgi
bool MusicMgr::GetUrlVailedMusic(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
    Json::Value& content = result["result"];
    std::list<int64> songid_list;
	packet::HttpPacket pack = packet;
	r = robot_storage::DBComm::GetVailedUrlMusic(songid_list);
	if(!r){
	    result["status"] = 0;
	    result["msg"] = "无新增歌曲";
	    content = "1";
	    goto ret;
	}

	GetBatchMusicInfo(songid_list,content);
    result["status"] = 1;
    result["msg"] = "";
ret:
	reponse = wr.write(result);
    r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
   		 reponse.length());
    return r;
}

//addvailedurlmusic.fcgi
bool MusicMgr::UpdateVailedMusic(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string reponse;
    Json::Value result;
    Json::Value response_result;
    Json::FastWriter wr;
    std::string ext_msg;
    Json::Value& content = result["result"];
    if (!pack.GetAttrib("content", ext_msg)) {
    	response_result["status"] = 0;
    	response_result["msg"] = "无新增歌曲";
    }

    UpdateVailedMusicInfo(ext_msg);
    response_result["status"] = 1;
ret:
	reponse = wr.write(response_result);
	r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
       		 reponse.length());
	return r;
}

//addvailedlyric.fcgi
bool MusicMgr::UpdateVailedLyric(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string reponse;
    Json::Value result;
    Json::Value response_result;
    Json::FastWriter wr;
    std::string ext_msg;
    Json::Value& content = result["result"];
    if (!pack.GetAttrib("content", ext_msg)) {
    	response_result["status"] = 0;
    	response_result["msg"] = "无新增歌词";
    }

    UpdateVailedLyric(ext_msg);
    response_result["status"] = 1;
ret:
	reponse = wr.write(response_result);
	r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
       		 reponse.length());
	return r;
}

//add_music.fcgi
bool MusicMgr::AddNewMusic(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string reponse;
    Json::Value result;
    Json::Value response_result;
    Json::FastWriter wr;
    std::string ext_msg;
    Json::Value& content = result["result"];
    if (!pack.GetAttrib("content", ext_msg)) {
    	response_result["status"] = 0;
    	response_result["msg"] = "无新增歌曲";
    }

    AddNewMusicInfo(ext_msg);
    response_result["status"] = 1;
ret:
	reponse = wr.write(response_result);
	r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
       		 reponse.length());
	return r;
}
//new_music.fcgi
// no param
bool MusicMgr::GetNewMusic(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
    Json::Value& content = result["result"];
	std::list<robot_base::NewMusicInfo> list;
	packet::HttpPacket pack = packet;
	r = robot_storage::DBComm::GetNewMusicInfo(list);
	if(!r||list.size()==0){
	    result["status"] = 0;
	    result["msg"] = "无新增歌曲";
	    content = "1";
	    goto ret;
	}

	ChargeNewMusciJson(list,content);
    result["status"] = 1;
    result["msg"] = "";
ret:
	reponse = wr.write(result);
    r =  robot_logic::SomeUtils::SendFull(socket, reponse.c_str(),
   		 reponse.length());
    return r;

}

void MusicMgr::GetBatchMusicInfo(std::list<int64>& songid_list,Json::Value& value){
	std::list<std::string> songinfo_list;
	std::list<robot_base::NewMusicInfo> new_music_list;
	robot_storage::RedisComm::GetBachMusicInfos(songid_list,songinfo_list);
	if(songinfo_list.size()==0)
		return;
	while(songinfo_list.size()>0){
		std::string songinfo = songinfo_list.front();
		songinfo_list.pop_front();
		robot_base::NewMusicInfo new_music;
		new_music.UnserializedJson(songinfo);
		new_music_list.push_back(new_music);
	}
	ChargeNewMusciJson(new_music_list,value);
}

void MusicMgr::ChargeNewMusciJson(std::list<robot_base::NewMusicInfo>& list, Json::Value& value){
	while(list.size()>0){
		robot_base::NewMusicInfo musicinfo = list.front();
		list.pop_front();
		Json::Value  info;
		info["id"] = (Json::Int64)(musicinfo.id());
		info["name"] = musicinfo.name().c_str();
		info["singer"] = musicinfo.singer().c_str();
		value.append(info);
	}
}

void MusicMgr::UpdateVailedLyric(const std::string& content){
	Json::Reader reader;
	Json::Value  root;
	LOG_DEBUG2("%s",content.c_str());
	bool r = reader.parse(content,root);
	if (!r)
		return ;
	int32 content_size = root.size();
	if(content_size<=0)
		return ;
	for (int i = 0;i<content_size;i++){
		int64 id = root[i]["id"].asInt64();
		std::string lyric = root[i]["lyric"].asString();
		robot_storage::DBComm::UpdateLyric(id,lyric);
	}
}

void MusicMgr::UpdateVailedMusicInfo(const std::string& content){
	Json::Reader reader;
	Json::Value  root;
	LOG_DEBUG2("%s",content.c_str());
	bool r = reader.parse(content,root);
	if (!r)
		return ;
	int32 content_size = root.size();
	if(content_size<=0)
		return ;
	for (int i = 0;i<content_size;i++){
		int64 id = root[i]["id"].asInt64();
		std::string mp3_url = root[i]["url"].asString();
		robot_storage::DBComm::UpdateMusicUrl(id,mp3_url);
	}
}

void MusicMgr::AddNewMusicInfo(const std::string& content){
	Json::Reader reader;
	Json::Value  root;
	LOG_DEBUG2("%s",content.c_str());
	bool r = reader.parse(content,root);
	if (!r)
		return ;
	int32 content_size = root.size();
	if(content_size<=0)
		return ;
	for (int i = 0;i<content_size;i++){
		int64 id = root[i]["id"].asInt64();
		std::string album = root[i]["album"].asString();
		std::string artist = root[i]["artist"].asString();
		std::string name = root[i]["name"].asString();
		std::string pubtime = root[i]["pubtime"].asString();
		std::string pic = root[i]["pic"].asString();
		std::string mp3_url = root[i]["url"].asString();
		AddNewMusic(id,name,album,artist,pubtime,pic,mp3_url);
	}
}
void MusicMgr::AddNewMusic(const int64 id,const std::string& name,const std::string& album,const std::string& artist,
		const std::string& pubtime,const std::string& pic,const std::string& url){
	// 加入数据库并返回歌曲ID
	std::string songid;
	bool r = robot_storage::DBComm::AddMusicInfo(id,name,album,artist,pubtime,pic,url,songid);
	if(!r)
		return;
	//加入redis
	r = robot_storage::RedisComm::AddNewMusicInfo(songid,album,name,pubtime,artist,pic);
}

}
