#ifndef _MIG_WX_PACKET_H__
#define _MIG_WX_PACKET_H__
#include "xmpp/xml_serialization.h"
#include "xmpp/xmppstanzaparser.h"
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

namespace weixin_base{

class WXPacket{
public:
	WXPacket();
	~WXPacket(){};
	void SetContent(const std::string content);

	void PutAttrib(const std::string& strName,const std::string& strValue);
	
	bool GetAttrib(const std::string& strName,std::string& strValue);

	bool GetPacketType(std::string& strValue);

private:
	std::map<std::string,std::string>  map_value_;

private:
	class StanzaParseHandler:public base::XmppStanzaParseHandler{
	public:
		StanzaParseHandler(WXPacket* outer):outer_(outer){}
		virtual void StartStream(const base::XmlElement * pelStream)
		{outer_->IncomingStart(pelStream);}
		virtual void Stanza(const base::XmlElement * pelStream)
		{outer_->IncomingStanza(pelStream);}
		virtual void EndStream()
		{outer_->IncomingEnd(false);}
		virtual void XmlError()
		{outer_->IncomingEnd(true);}
	private:
		WXPacket* const outer_;
	};

friend class StanzaParseHandler;
private:
	StanzaParseHandler stanzaParseHandler_;
	base::XmppStanzaParser stanzaParser_;
private:
	void IncomingStanza(const base::XmlElement* pelStanza);
	void IncomingStart(const base::XmlElement* pelStanza);
	void IncomingEnd(bool isError);
};

}
#endif
