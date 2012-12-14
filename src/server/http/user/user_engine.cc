#include "user_engine.h"
#include "log/mig_log.h"
//#include "xmlelement.h"

namespace userinfo{

UserEngine::UserEngine()
/*:stanzaHandler_(this)
,stanzaParser_(&stanzaHandler_)*/{

}

UserEngine::~UserEngine(){

}

bool UserEngine::HandlerInput(const char* data,const int len){
	std::string head;
	/*head = "<stream:stream from=\"gmail.com\" " 
			"id=\"1F83A90940271513\" " 
			"version=\"1.0\" " 
			"xmlns:stream=\"http://etherx.jabber.org/streams\" " 
			"xmlns=\"jabber:client\">";

 	stanzaParser_.Parse(head.c_str(),head.length(),false);

	std::string content;
	content = "<stream:features>"
				"<starttls xmlns=\"urn:ietf:params:xml:ns:xmpp-tls\">"
				"<required/>"
				"</starttls>"
				"<mechanisms xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
				"<mechanism>X-GOOGLE-TOKEN</mechanism><"
				"mechanism>X-OAUTH2</mechanism>"
				"</mechanisms>"
			"</stream:features>";
	stanzaParser_.Parse(content.c_str(),content.length(),false);*/
	return true;
}

/*
void UserEngine::IncomingStart(const base::XmlElement* pelStream){

}

void UserEngine::IncomingStanza(const base::XmlElement* pelStanza){
	//if (pelStanza->Name()==base::QN_STREAM_FEATURES)
		//return ;
}

void UserEngine::IncomingEnd(bool isError){
	//if(isError)
	//	MIG_ERROR(USER_LEVEL,"ERROR_XML");
	//else
	//	MIG_ERROR(USER_LEVEL,"ERROR_DOUCMENT_CLOSED");
		
}*/

}
