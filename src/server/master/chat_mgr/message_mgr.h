#ifndef _MASTER_PLUGIN_CHAT_MGR_MESSAGE_MGR__H__
#define _MASTER_PLUGIN_CHAT_MGR_MESSAGE_MGR__H__
#include "logic_comm.h"
#include "plugins.h"
#include "basic/http_packet.h"
#include "json/json.h"
#include <list>
namespace chat_logic{

class MessageMgr{
public:
	MessageMgr();
	virtual ~MessageMgr();
public:
	bool GetLeaveMessage(const int socket,const packet::HttpPacket& packet);
private:
	bool PacketLeaveMessage(std::list<struct GetLeaveMessage*> list,Json::Value& chat_info);
};
}
#endif
