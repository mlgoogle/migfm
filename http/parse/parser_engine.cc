#include "parser_engine.h"
#include "log/mig_log.h"
#include "basic/basic_util.h"
#include "xmpp/xmlelement.h"
#include <sstream>
ParserEngineImpl::ParserEngineImpl()
:stanzaParserHandler_(this)
,stanzaParser_(&stanzaParserHandler_){

    std::string head = "<stream:stream from=\"gmail.com\" " 
                       "id=\"1F83A90940271513\" "
                       "version=\"1.0\" " 
                       "xmlns:stream=\"http://etherx.jabber.org/streams\" " 
                       "xmlns=\"jabber:client\">";
    stanzaParser_.Parse(head.c_str(),head.length(),false);
}

void ParserEngineImpl::IncomingStanza(const base::XmlElement* pelStanza){
    std::string name = pelStanza->Name().LocalPart();
    MIG_DEBUG(USER_LEVEL,"name[%s]",name.c_str());
    const base::XmlAttr* xmlattr = pelStanza->FirstAttr();
    do{
       std::string attr_name = xmlattr->Name().LocalPart();
       std::string attr_value = xmlattr->Value();
       MIG_DEBUG(USER_LEVEL,"attr_name[%s],attr_value[%s]",
                 attr_name.c_str(),attr_value.c_str());
    }while((xmlattr=xmlattr->NextAttr())!=NULL);

    const base::XmlChild* child  = pelStanza->FirstChild();
    do{
        std::stringstream os;
        const base::XmlAttr* child_attr = child->AsElement()->FirstAttr();
        do{
            std::string name = child_attr->Name().LocalPart();
            std::string value = child_attr->Value();
            os<<name<<":"<<base::BasicUtil::GetSrcString(value)<<" ";
        }while((child_attr = child_attr->NextAttr())!=NULL);
        MIG_DEBUG(USER_LEVEL,"os[%s]",os.str().c_str());
    }while((child=child->NextChild())!=NULL);
    MIG_DEBUG(USER_LEVEL,"======================"); 
}

void ParserEngineImpl::IncomingStart(const base::XmlElement* pelStanza){

}

void ParserEngineImpl::IncomingEnd(bool isError){}

void ParserEngineImpl::ParserXml(const std::string& content){
     stanzaParser_.Parse(content.c_str(),content.length(),false);
}

ParserEngine* ParserEngine::Create(){
    return new ParserEngineImpl;
}
