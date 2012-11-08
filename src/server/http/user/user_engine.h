#ifndef MIG_FM_USER_ENGINE_H__
#define MIG_FM_USER_ENGINE_H__
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "xmppstanzaparser.h"
#include "xmlelement.h"
#include "qname.h"
#include "constants.h"
namespace userinfo{

class base::XmlElement;

class UserEngine{
public:
	UserEngine();
	~UserEngine();
	
	bool HandlerInput(const char* data,const int len); 
private:
	class StanzaParseHandler: public base::XmppStanzaParseHandler{
	public:
		StanzaParseHandler(UserEngine* outer) : outer_(outer){}
		virtual void StartStream(const base::XmlElement* pelStream)
			{outer_->IncomingStart(pelStream);}
		virtual void Stanza(const base::XmlElement* pelStanza)
			{outer_->IncomingStanza(pelStanza);}
		virtual void EndStream()
			{outer_->IncomingEnd(false);}
		virtual void XmlError()
			{outer_->IncomingEnd(true);}
	private:
		UserEngine* const outer_;
	};
	
	friend class StanzaParseHandler;
	void IncomingStart(const base::XmlElement* pelStream);
	void IncomingStanza(const base::XmlElement* pelStanza);
	void IncomingEnd(bool isError);
private:
	base::XmppStanzaParser  stanzaParser_;
	StanzaParseHandler stanzaHandler_;
	
};
}
#endif
