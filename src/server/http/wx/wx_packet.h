#ifndef _MIG_WX_PACKET_H__
#define _MIG_WX_PACKET_H__
#include <map>
#include <string>

#define  MsgType       "MsgType"
#define  ToUserName    "ToUserName"
#define  FromUserName  "FromUserName"
#define  CreateTime    "CreateTime"
#define  Content       "Content"
#define  MsgId         "MsgId"
#define  Event         "Event"
#define  EventKey      "EventKey"

namespace wxinfo{

class WXPacket{
public:
	WXPacket(){};
	~WXPacket(){};
	void PutAttrib(const std::string& strName,const std::string& strValue);
	
	bool GetAttrib(const std::string& strName,std::string& strValue);

	bool GetPacketType(std::string& strValue);

private:
	std::map<std::string,std::string>  map_value_;
};

}
#endif