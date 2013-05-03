#include "wx_engine.h"
#include "log/mig_log.h"
#include "config/config.h"
#include "storage/db_serialization.h"
#include "storage/dic_serialization.h"
#include "basic/basic_util.h"
#include "xmpp/xmlelement.h"
#include "basic/constants.h"
#include "http_response.h"
#include "json/json.h"
#include <sstream>
namespace wxinfo{


static void FindSpString(const char* str,int len,const char c,std::string& dest_string){
	for (int index =0; index<len;index++){
		if (str[index]==c)
			MIG_DEBUG(USER_LEVEL,"========");
		else
			dest_string.append(1,str[index]);
	}
	MIG_DEBUG(USER_LEVEL,"%s",dest_string.c_str());
}

wxinfo::WXInfoEngine* WXInfoEngine::engine_ = NULL;
WXInfoEngine::WXInfoEngine()
:stanzaParseHandler_(this)
,stanzaParser_(&stanzaParseHandler_){
	std::string head;


	head = "<stream:stream from=\"gmail.com\" " 
		"id=\"1F83A90940271513\" "  //通过SSO获取 
		"version=\"1.0\" " 
		"xmlns:stream=\"http://etherx.jabber.org/streams\" " 
		"xmlns=\"jabber:client\">";
	stanzaParser_.Parse(head.c_str(),head.length(),false);
	url_ = "http://sandbox.api.simsimi.com/request.p?key=e40e902e-302c-49f9-bb48-b4d1c2fa84c8&lc=ch&ft=1.0&text=";
}



WXInfoEngine::~WXInfoEngine(){

}

WXInfoEngine* WXInfoEngine::GetEngine(){
    if(engine_==NULL){
        engine_ = new WXInfoEngine();
    }
    return engine_;
}

void WXInfoEngine::FreeEngine(){
    if(engine_!=NULL){
        delete engine_;
        engine_ = NULL;
    }
}

bool WXInfoEngine::InitEngine(std::string& path){
    bool r = false;
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    if(config==NULL){
        return r;
    }
    r = config->LoadConfig(path);
    if(!r)
    	return r;
    r = base_storage::MysqlSerial::Init(config->mysql_db_list_);
    if(!r)
        return r;
    r = base_storage::MemDicSerial::Init(config->mem_list_);
    if(!r)
        return r;
    r = base_storage::RedisDicSerial::Init(config->redis_list_);
    if(!r)
        return r;
    return r;
}

void 
WXInfoEngine::PostContent(const std::string& content){
	MIG_DEBUG(USER_LEVEL,"\n%s\n",content.c_str());
	std::string dest_string;
	FindSpString(content.c_str(),content.length(),'\n',dest_string);
	stanzaParser_.Parse(dest_string.c_str(),dest_string.length(),false);
}

void 
WXInfoEngine::IncomingStanza(const base::XmlElement *pelStanza){
	WXPacket packet;
	const base::XmlChild* child = pelStanza->FirstChild();
	do {
		std::string name = child->AsElement()->Name().LocalPart();
		std::string name_content = child->AsElement()->FirstChild()->AsText()->Text();
		packet.PutAttrib(name,name_content);
	} while ((child=child->NextChild())!=NULL);
	ProcessMsg(packet);
}

void
WXInfoEngine::IncomingStart(const base::XmlElement* pelStanza){

}

void 
WXInfoEngine::IncomingEnd(bool isError){

}

void 
WXInfoEngine::ProcessMsg(WXPacket& msg){
	std::string msgtype;
 	bool r = msg.GetPacketType(msgtype);
 	if (!r)
 		return;
	if (msgtype=="text")
		ProcessMsgText(msg);
	return ;
}

/*
<xml>
<ToUserName><![CDATA[toUser]]></ToUserName>
<FromUserName><![CDATA[fromUser]]></FromUserName>
<CreateTime>12345678</CreateTime>
<MsgType><![CDATA[text]]></MsgType>
<Content><![CDATA[content]]></Content>
<FuncFlag>0</FuncFlag>
</xml>
*/
void
WXInfoEngine::ProcessMsgText(WXPacket& msg){
	std::string to_user;
	std::string from_user;
	std::string content_s;
	std::string  content;
	Json::Reader reader;
	Json::Value  root;

	msg.GetAttrib(ToUserName,to_user);
	msg.GetAttrib(FromUserName,from_user);
	msg.GetAttrib(Content,content_s);

	//std::stringstream os;
	//os<<url_<<content_s;
	//std::string url = os.str();
	//wxinfo::HttpResponse http(url);
	//http.Get();
	//http.GetContent(content);

	//bool r = reader.parse(content.c_str(),root);
	//if (!r){
	//	MIG_ERROR(USER_LEVEL,"parse json error[%s]",content.c_str());
	//	return ;
	//}

	//int ack_code = root["result"].asInt();
	//if (ack_code==100){
	//	std::string reponse = root["response"].asString();
	///*char*  output;
	//size_t output_len;
	//base::BasicUtil::GB2312ToUTF8(content_s.c_str(),content_s.length(),
	//								&output,&output_len);
	//std::string utf8content;
	//utf8content.assign(output,output_len);*/

	//	PackageTextMsg(from_user,to_user,reponse);
	//}

}

void
WXInfoEngine::PackageTextMsg(std::string& to_user,std::string& from_user, std::string& content){
	std::stringstream os;
	os<<"<xml>"
		<<"<ToUserName><![CDATA["<<to_user<<"]]></ToUserName>"
		<<"<FromUserName><![CDATA["<<from_user<<"]]></FromUserName>"
		<<"<CreateTime>"<<time(NULL)<<"</CreateTime>"
		<<"<MsgType><![CDATA[text]]></MsgType>"
		<<"<Content><![CDATA["<<content.c_str()<<"]]></Content>"
		<<"<FuncFlag>0</FuncFlag>"
		"</xml>";
	content_ = os.str().c_str();
}

}
