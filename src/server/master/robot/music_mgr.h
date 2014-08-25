#ifndef _MASTER_PLUGIN_ROBOT_MUSIC_MGR__H__
#define _MASTER_PLUGIN_ROBOT_MUSIC_MGR__H__
#include "logic_comm.h"
#include "robot_basic_infos.h"
#include "plugins.h"
#include "protocol/http_packet.h"
#include "json/json.h"
#include <list>
namespace robot_logic{

class MusicMgr{
public:
	MusicMgr();
	virtual ~MusicMgr();
public:
	bool GetNewMusic(const int socket,const packet::HttpPacket& packet);
	bool AddNewMusic(const int socket,const packet::HttpPacket& packet);
	bool GetUrlVailedMusic(const int socket,const packet::HttpPacket& packet);
	bool UpdateVailedMusic(const int socket,const packet::HttpPacket& packet);
	bool UpdateVailedLyric(const int socket,const packet::HttpPacket& packet);
	bool GetVailedLyric(const int socket,const packet::HttpPacket& packet);
private:
	void GetBatchMusicInfo(std::list<int64>& songid_list,Json::Value& value);
	void ChargeNewMusciJson(std::list<robot_base::NewMusicInfo>& list, Json::Value& value);
	void AddNewMusicInfo(const std::string& content);
	void UpdateVailedMusicInfo(const std::string& content);
	void UpdateVailedLyric(const std::string& content);
	void AddNewMusic(const int64 id,const std::string& name,const std::string& album,const std::string& artist,
			const std::string& pubtime,const std::string& pic,const std::string& url);
	//bool AddNewMusicAllInfo(const int socket,const packet::HttpPacket& packet);
private:
	int64              from_;
	int64              count_;
};
}
#endif
