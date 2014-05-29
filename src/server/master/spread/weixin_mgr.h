#ifndef _MASTER_PLUGIN_WEIXIN__H__
#define _MASTER_PLUGIN_WEIXIN__H__
#include "weixin_packet.h"
#include "plugins.h"
#include "spread_basic_infos.h"
#include "protocol/http_packet.h"
#include "xmpp/xml_serialization.h"
#include "xmpp/xmppstanzaparser.h"
#include <list>
namespace spread_logic{

/*
class WeiXinEventMgr{
public:
	WeiXinEventMgr();
	~WeiXinEventMgr();
	bool WeiXinEventMessage(const int socket,weixin_base::WXPacket& packet);
private:
	bool WeiXinEventClickMessage(const int socket,weixin_base::WXPacket& packet);

};
*/

class WeiXinMgr{
public:
	WeiXinMgr();
	virtual ~WeiXinMgr();
public:
	void Init();
public:
	bool Message(const int socket,const packet::HttpPacket& packet);
private:
	bool WeiXinMessage(const int socket,weixin_base::WXPacket& packet);
	void OnTestMsg(const int socket,weixin_base::WXPacket& packet);
	void OnEventMsg(const int socket,weixin_base::WXPacket& packet);
	void OnClick(const int socket,weixin_base::WXPacket& packet);
	void OnView(const int socket,weixin_base::WXPacket& packet);
private:
	void OnReadMeEvenet(const int socket,weixin_base::WXPacket& packet);
	void OnOptinionEvent(const int socket,weixin_base::WXPacket& packet);
private:
	void CreateToken();
	void CreateMenu();
	void PackageTextMsg(const std::string& to_user,const std::string& from_user,
				const std::string& content,std::string& out_content);
	void PacketGraphicTextMsg(const std::string& to_user,const std::string& from_user,
					std::list<base::WeiXin::GraphicTextInfo>& list,std::string& out_content);
private:
	std::string               access_token_;
	int32                     exp_time_;
};

}

#endif
