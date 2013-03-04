#include "http_engine_impl.h"
#include "net_comm/http_engine.h"
#include "net_comm/http_constants.h"
#include "net_comm/httplogintask.h"
#include "net_comm/xmpp/xmlelement.h"
#include "net_comm/xmpp/constants.h"
#include "googleurl/src/gurl.h"

#include <map>
#include <string.h>

namespace net_comm{

HttpEngine* HttpEngine::Create(){
	return  new HttpEngineImpl;
}

HttpEngineImpl::HttpEngineImpl()
:hid_(HID_EMPTY)
,stanzaParseHandler_(this)
,stanzaParser_(&stanzaParseHandler_)
,resource_()
,requested_resource_()
,output_(new std::stringstream())
,output_handler_(NULL)
,session_handler_(NULL)
,state_(STATE_START)
,error_(ERROR_NONE){
  
	login_task_.reset(new HttpLoginTask(this));
	std::string head;


	head = "<stream:stream from=\"gmail.com\" " 
		"id=\"1F83A90940271513\" "  //Í¨¹ýSSO»ñÈ¡ 
		"version=\"1.0\" " 
		"xmlns:stream=\"http://etherx.jabber.org/streams\" " 
		"xmlns=\"jabber:client\">";

	stanzaParser_.Parse(head.c_str(),head.length(),false);
}

HttpEngineImpl::~HttpEngineImpl(){

}


HttpEngineImpl::EnterExit::EnterExit(net_comm::HttpEngineImpl *engine)
:engine_(engine)
,state_(engine->state_)
,error_(engine->error_){
	engine->engine_entered_+=1;
}

HttpEngineImpl::EnterExit::~EnterExit(){
	HttpEngineImpl* engine = engine_;
	engine_->engine_entered_-=1;
	int closeing = 0;
	std::string content;
	int32 code;
	std::string url = engine_->output_->str();
	if (url.length()>0&&engine_->output_handler_){
// 		//post
//   		std::string post_content = "RequestID=_3EB2891D82973C01EEB750C3BBF49432&MajorVersion=1&MinorVersion=2&IssueInstant=2013-02-18T15%3A22%3A03Z&ProviderID=http%3A%2F%2Ffm.miglab.com&ForceAuthn=false&IsPassive=false&NameIDPolicy=federated&RelayState=fake%5B%5D&consent=urn%3Aliberty%3Aconsent%3Aobtained&SigAlg=http%3A%2F%2Fwww.w3.org%2F2000%2F09%2Fxmldsig%23rsa-sha1&Signature=1tDp7%2Fm9ufYW%2B7ji%2Bdnh%2FR3IV9qTQjQ67SrnBNYiEuOM7cHU1a%2FkXhUKNkGljB%2BIjkMbH8ekS1kr16gmEnhZQ1MMThUGmAP0sxpgFIy4G33Dul7ddQ0%2BW42TJ1jq8FzNVRScGydayzdXC6EIMDSveLyAeOG43eIRfWtcGJxHCULQ8eRD4J44mUMEz2wtjYycytt%2FCAX7NrGgd9eF0mGT4UNA%2FSZfEYO5DjCoK7%2FKwRKwZGKD0J3sdbwNOYrsdqkdPDEZcoOk58wZylNFNnCNGLnXMW2Sqlf2QsJv%2Fq4a%2BzOwLRGqn4YrBLZYGNl9m9%2Bv%2BNM5yfjJWPKd2Nk4UxtiYA%3D%3D&username=flaght@gmail.com&email=myemail@163.com&password=mypassword&autologin=1";
//   		url = "http://sso.miglab.com/cgi-bin/idp.fcgi";
//  		engine_->output_handler_->WriteOutput(url,post_content,content,code);

 		/*url = "http://42.121.112.248/cgi-bin/record.fcgi";
// 		
 		std::string post_content = "{\"name\":\"flaght@gmail.com\",\"phone\":\"18668169052\",\"enter\":\"3\",\"music\":[{\"name\":\"ÊÄÑÔ1\", \"singer\":\"Íõ·Æ1\"},{\"name\":\"Äº´ºÇïÉ«\",\"singer\":\"ñ¼Î¨\"},{\"name\":\"Don't Break My Heart\",\"singer\":\"ºÚ±ªÀÖ¶Ó\"}]}";
		engine_->output_handler_->WriteOutput(url,post_content,content,code);*/
	/*	engine_->output_handler_->WriteOutput(url,content,code);
		int pos = content.find("?");
		std::string post_url = content.substr(0,pos);
		std::string post_content = content.substr(pos+1,content.length());
		post_content ="username=flaght@gmail.com&password=19860903&"+post_content;
		engine_->output_handler_->WriteOutput(post_url,post_content,content,code);


		std::string post_content1;
		pos = content.find("?");
		std::string ticket_url = content.substr(0,pos);
		std::string ticket_content = content.substr(pos+1,content.length());

		engine_->output_handler_->WriteOutput(ticket_url,ticket_content,post_content1,code);*/

		/*std::string request1 = "userid=10001&pass=112232&token=AAOfv3WG35avZspzKhoeodwv2MFd8zgyQzc5ODg5NTFENjAyQUU1M0VF";
		std::map<std::string,std::string> http_map;
		while (request1.length()!=0){
			int32 start_pos = request1.find("=");
			int32 end_pos = request1.find("&")==-1?request1.length():request1.find("&");
			std::string key = request1.substr(0,start_pos);
			std::string value = request1.substr(key.length()+1,end_pos - key.length()-1);
			http_map[key] = value;
			if (request1.find("&")!=-1)
				request1 = request1.substr(end_pos+1,request1.length());
			else
				request1.clear();
		}*/
		


/*
		int32 token_pos = request1.find("token");
		std::string temp_str = request1.substr(token_pos,request1.length());
		int32 and_pos = temp_str.find("&");
		int32 end_pos = and_pos>0?(and_pos-6):(request1.length()-6);
		std::string token = temp_str.substr(6,end_pos);


		std::string username;
		std::string password;
		std::string requestid;
		std::string all = "username=flaght@gmail.com&password=19860903&RequestID=_E46DAC91CE162E3F17315C08E7A1C970&MajorVersion=1&MinorVersion=2&IssueInstant=2013-02-18T15%3A39%3A50Z&ProviderID=http%3A%2F%2Ffm.miglab.com&ForceAuthn=false&IsPassive=false&NameIDPolicy=federated&RelayState=fake%5B%5D&consent=urn%3Aliberty%3Aconsent%3Aobtained&SigAlg=http%3A%2F%2Fwww.w3.org%2F2000%2F09%2Fxmldsig%23rsa-sha1&Signature=3R3IPakh9qSlWsv%2Bf6gOiJGH6e%2FkJyHZmVwzqC3RDw4HmAyxYPwWFjRJP1Wn7%2B41auZxN590tuJaNyspRHgXG4LVXbRxojJ9HWsuNJlZ0FKIzDx1UIbCAIAWrUxPkFI%2F2M2BPZvYmlXlN18%2B%2BtydpxruE1jbTBQodsO8GqdasBqYAUw%2FdfBVq8GcS2fLsSgQGdku%2BT7hTF5Pj2rscOjVrAq6Y2bRWcCHO8znRkK%2Byu15UI1856LThwGrpUQzxR6OvAH21iN0pdLgUPxrZc%2B6%2BujNwwU8%2BaebgfAMBh5tWz%2BMGPcTDJT1DCw7no2X3EeXef2HOfeTjV5w6QFhwNajjw%3D%3D";
	    int first = all.find("username");
		int send = all.find("&");
		username = all.substr(first+9,send-9);


		std::string alltwo;
		alltwo = all.substr(send+1,all.length());
		first = alltwo.find("password");
		send = alltwo.find("&");
		password =alltwo.substr(first+9,send-9);

		std::string  request = alltwo.substr(send+1,all.length());*/

	}
	///////////////////////////////test////////////////////////////////////
 
// 
// 	std::string content;
// 	content = "<stream:features>"
// 				"<starttls xmlns=\"urn:ietf:params:xml:ns:xmpp-tls\">"
// 				"<required/>"
// 				"</starttls>"
// 				"<mechanisms xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
// 				"<mechanism>X-GOOGLE-TOKEN</mechanism><"
// 				"mechanism>X-OAUTH2</mechanism>"
// 				"</mechanisms>"
// 			"</stream:features>";
// 
// 	//stanzaParser_.Parse(content.c_str(),content.length(),false);
 	engine_->stanzaParser_.Parse(content.c_str(),content.length(),false);
// 	engine_->output_->str("");
	if (closeing){
		//engine_->output_handler_->CloseConnection();
		engine_->output_handler_ = NULL;
	}

	if (engine_->engine_entered_)
		return;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetOutputHandler(net_comm::HttpOutPutHandler *phoh){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	output_handler_ = phoh;
	return HTTP_RETURN_OK;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetUser(const net_comm::Hid &hid){
	if(state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	hid_ = hid;
	return HTTP_RETURN_OK;
}

const net_comm::Hid& HttpEngineImpl::GetUser() const{
	return hid_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetPassword(const std::string& password){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	password_ = password;
	return HTTP_RETURN_OK;
}

const std::string& HttpEngineImpl::GetPassword() const{
	return password_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::SetRequestedResource(const std::string &resource){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	resource_ = resource;
	return HTTP_RETURN_OK;
}

const std::string& HttpEngineImpl::GetRequestedResource() const{

	return resource_;
}

HttpEngine::HttpReturnStatus HttpEngineImpl::OnUsrLogin(){
	if (state_!=STATE_START)
		return HTTP_RETURN_BADARGUMENT;
	EnterExit ee(this);
	state_ = STATE_OPENING;
	if (login_task_.get()){
		login_task_->IncomingStanza(false);
		if (login_task_->IsDone())
			login_task_.reset();
	}

	return HTTP_RETURN_OK;
}

void 
HttpEngineImpl::InternalRequestLogin(const std::string& password){
	//*output_<<"http://www.9see.com/ashx/getlist.ashx";
	//*output_<<"http://42.121.112.248/cgi-bin/show.fcgi?userid=10001";
	//*output_<<"http://60.191.220.135/cgi-bin/echo1.fcgi";
	*output_<<"http://sso.miglab.com/cgi-bin/sp.fcgi?sp";
}


void 
HttpEngineImpl::InternalGetUserInfoSelf(const std::string& uid){
	//*output_<<"http://42.121.112.248/cgi-bin/show.fcgi?userid="<<uid<<";";
	//*output_<<"http://60.191.220.135/cgi-bin/echo1.fcgi";
}

void 
HttpEngineImpl::IncomingStanza(const base::XmlElement *pelStanza){
	if (pelStanza->Name()==base::QN_STREAM_FEATURES)
		return ;
}

void
HttpEngineImpl::IncomingStart(const base::XmlElement* pelStanza){

}

void HttpEngineImpl::IncomingEnd(bool isError){

}

}