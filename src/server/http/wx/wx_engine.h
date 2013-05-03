#ifndef MIG_WX_ENGINE_H__
#define MIG_WX_ENGINE_H__
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "basic/constants.h"
#include "log/mig_log.h"
#include "basic/scoped_ptr.h"
#include "storage/db_serialization.h"
#include "storage/dic_serialization.h"
#include "xmpp/xml_serialization.h"
#include "xmpp/xmppstanzaparser.h"
#include "wx_packet.h"
namespace wxinfo{

class WXInfoEngine{
public:
    WXInfoEngine();
    ~WXInfoEngine();
    bool InitEngine(std::string& path);

	void PostContent(const std::string& content);
	std::string& GetProcessContent(){return content_;}
	class StanzaParseHandler:public base::XmppStanzaParseHandler{
	public:
		StanzaParseHandler(WXInfoEngine* outer):outer_(outer){}
		virtual void StartStream(const base::XmlElement * pelStream)
		{outer_->IncomingStart(pelStream);}
		virtual void Stanza(const base::XmlElement * pelStream)
		{outer_->IncomingStanza(pelStream);}
		virtual void EndStream()
		{outer_->IncomingEnd(false);}
		virtual void XmlError()
		{outer_->IncomingEnd(true);}
	private:
		WXInfoEngine* const outer_;
	};
friend class StanzaParseHandler;
private:
	StanzaParseHandler stanzaParseHandler_;
	base::XmppStanzaParser stanzaParser_;
private:
	void IncomingStanza(const base::XmlElement* pelStanza);
	void IncomingStart(const base::XmlElement* pelStanza);
	void IncomingEnd(bool isError);
	void ProcessMsg(WXPacket& msg);
	void ProcessMsgText(WXPacket& msg);
	void PackageTextMsg(std::string& to_user,std::string& from_user,
		                std::string& content);
public: 
    static WXInfoEngine* GetEngine();
    static void  FreeEngine();
private:
    static wxinfo::WXInfoEngine* engine_;
	std::string  content_;
	std::string  url_;
private:

};
}
#endif
