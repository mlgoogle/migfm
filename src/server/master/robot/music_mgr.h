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
private:
	void ChargeNewMusciJson(std::list<robot_base::NewMusicInfo>& list, Json::Value& value);
	void AddNewMusicInfo(const std::string& content);
	void AddNewMusic(const int64 id,const std::string& name,const std::string& album,const std::string& artist,
			const std::string& pubtime,const std::string& pic,const std::string& url);
	//bool AddNewMusicAllInfo(const int socket,const packet::HttpPacket& packet);
};
}
#endif
