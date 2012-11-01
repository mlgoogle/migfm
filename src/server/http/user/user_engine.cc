#include "user_engine.h"
#include "mig_log.h"
#include "xmlelement.h"

namespace userinfo{

UserEngine::UserEngine()
:stanzaHandler_(this)
,stanzaParser_(&stanzaHandler_){

}

UserEngine::~UserEngine(){

}

bool UserEngine::HandlerInput(const char* data,const int len){
	stanzaParser_.Parse(data,len,false);
	return true;
}

void UserEngine::IncomingStart(const base::XmlElement* pelStream){

}

void UserEngine::IncomingStanza(const base::XmlElement* pelStanza){
	MIG_ERROR(USER_LEVEL,"xml Name:[%s]",pelStanza->Name());

}

void UserEngine::IncomingEnd(bool isError){
	if(isError)
		MIG_ERROR(USER_LEVEL,"ERROR_XML");
	else
		MIG_ERROR(USER_LEVEL,"ERROR_DOUCMENT_CLOSED");
		
}

}
