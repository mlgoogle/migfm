#include "music_mgr.h"
#include "db_comm.h"
#include "dic_comm.h"
#include <list>
#include <string>

namespace robot_logic{

MusicMgr::MusicMgr(){

}

MusicMgr::~MusicMgr(){

}


//add_music.fcgi
bool MusicMgr::AddNewMusic(const int socket,const packet::HttpPacket& packet){
	bool r = false;
	packet::HttpPacket pack = packet;
	std::string reponse;
    Json::Value result;
    Json::FastWriter wr;
    std::string ext_msg;
    Json::Value& content = result["result"];
    if (!pack.GetAttrib("content", ext_msg)) {
	    result["status"] = 0;
	    result["msg"] = "无新增歌曲";
    }

    AddNewMusicInfo(ext_msg);
    result["status"] = 1;
ret:
	reponse = wr.write(result);
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
	if(!r){
	    result["status"] = 0;
	    result["msg"] = "无新增歌曲";
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
