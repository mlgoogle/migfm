#ifndef _MASTER_PLUGIN_ROBOT_UTIL_MGR__H__
#define _MASTER_PLUGIN_ROBOT_UTIL_MGR__H__
#include "logic_comm.h"
#include "robot_basic_infos.h"
#include "plugins.h"
#include "protocol/http_packet.h"
#include "json/json.h"
#include <list>
namespace robot_logic{

class UtilMgr{
public:
	UtilMgr();
	virtual ~UtilMgr();
public:
	bool GetSpreadMail(const int socket,const packet::HttpPacket& packet);
};
}
#endif
