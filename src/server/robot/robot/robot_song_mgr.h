#ifndef _ROBOT_ROBOT_SONG_MGR_H__
#define _ROBOT_ROBOT_SONG_MGR_H__
#include "robot_basic_info.h"
#include "logic_unit.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include "json/json.h"
#include "common.h"

namespace robot_logic{
class RobotSongMgr{
public:
	RobotSongMgr();
	virtual ~RobotSongMgr();
public:
	bool OnUserDefaultSong(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);

	bool OnRobotLoginSong(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg = NULL, int len = 0);
private:
	bool ResolveJsonMusic(const std::string& musicinfo,Json::Value& value);
};
}
#endif
