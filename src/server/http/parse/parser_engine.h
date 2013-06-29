#include <stdio.h>
#include <stdlib.h>
#include "xmpp/xmppstanzaparser.h"

class ParserEngine{
public:
    enum Error{
        ERROR_NONE = 0
    };
    ~ParserEngine(void) {}
    static ParserEngine* Create();
public:
    virtual void ParserXml(const std::string& content) = 0;
};
class ParserEngineImpl:public ParserEngine{
public:
    ParserEngineImpl();
    virtual ~ParserEngineImpl(){}
    void IncomingStanza(const base::XmlElement* pelStanza);
    void IncomingStart(const base::XmlElement* pelStanza);
    void IncomingEnd(bool isError);
    void ParserXml(const std::string& content);
    class StanzaParseHandler :public base::XmppStanzaParseHandler{
    public:
        StanzaParseHandler(ParserEngineImpl* outer):outer_(outer){}
        virtual void StartStream(const base::XmlElement* pelStream)
        { outer_->IncomingStart(pelStream);}
    
        virtual void Stanza(const base::XmlElement* pelStanza)
        { outer_->IncomingStanza(pelStanza);}
    
        virtual void EndStream()
        { outer_->IncomingEnd(false);}
    
        virtual void XmlError()
        { outer_->IncomingEnd(true);}
    private:
        ParserEngineImpl* const  outer_;
    };

friend class StanzaParseHandler;

   base::XmppStanzaParser stanzaParser_;
   StanzaParseHandler    stanzaParserHandler_;
};


