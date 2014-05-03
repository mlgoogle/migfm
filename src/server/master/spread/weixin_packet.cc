#include "weixin_packet.h"
#include "xmpp/xmlelement.h"
namespace weixin_base{


WXPacket::WXPacket()
:stanzaParseHandler_(this)
,stanzaParser_(&stanzaParseHandler_){
	std::string head;
	head = "<stream:stream from=\"gmail.com\" "
		"id=\"1F83A90940271513\" "  //ͨ��SSO��ȡ
		"version=\"1.0\" "
		"xmlns:stream=\"http://etherx.jabber.org/streams\" "
		"xmlns=\"jabber:client\">";
	stanzaParser_.Parse(head.c_str(),head.length(),false);
}

void WXPacket::SetContent(const std::string content){
	stanzaParser_.Parse(content.c_str(),content.length(),false);
}


void
WXPacket::IncomingStanza(const base::XmlElement *pelStanza){
	const base::XmlChild* child = pelStanza->FirstChild();
	do {
		std::string name = child->AsElement()->Name().LocalPart();
		if (name=="EventKey")
			continue;
		std::string name_content = child->AsElement()->FirstChild()->AsText()->Text();
		PutAttrib(name,name_content);
	} while ((child=child->NextChild())!=NULL);
}

void
WXPacket::IncomingStart(const base::XmlElement* pelStanza){

}

void
WXPacket::IncomingEnd(bool isError){

}



void WXPacket::PutAttrib(const std::string &strName, const std::string &strValue){
	
	map_value_[strName] = strValue;
}

bool WXPacket::GetAttrib(const std::string& strName,std::string& strValue){
	std::map<std::string,std::string>::iterator it = map_value_.find(strName);
	if (it!=map_value_.end()){
		strValue=(*it).second;
		return true;
	}
	return false;
}

bool WXPacket::GetPacketType(std::string &strValue){
	std::map<std::string,std::string>::iterator it = map_value_.find(MsgType);
	if (it!=map_value_.end()){
		strValue=(*it).second;
		return true;
	}
	return false;
}

}
