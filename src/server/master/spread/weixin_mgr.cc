#include "weixin_mgr.h"
#include "db_comm.h"
#include "logic_comm.h"
#include "spread_basic_infos.h"
#include "json/json.h"
#include "xmpp/xmlelement.h"
#include <list>
#include <string>

namespace spread_logic{

WeiXinMgr::WeiXinMgr(){
	Init();
}

WeiXinMgr::~WeiXinMgr(){

}

void WeiXinMgr::Init(){
	CreateToken();
	CreateMenu();
}

void WeiXinMgr::CreateToken(){
	bool r = false;
	std::string appid = "wxa784d1abb1d98bad";
	std::string secret = "1189440fb1aad9e740f599589f42cdec";
	std::string content;
	r = spread_logic::HttpComm::WeiXin::GetWeiXinToken(appid,secret,content);
	if(!r)
		return ;
	r = spread_logic::ResolveJson::WeiXin::ResolveWeiXinGetToken(content,access_token_,exp_time_);
	if(!r)
		return ;
}

void WeiXinMgr::CreateMenu(){
	bool r= false;
	std::string content;
	std::string result;
	std::stringstream os;
	os<<"{\"button\": [{\"type\": \"click\",\"name\": \"使用说明\",\"key\":"
		<<"\"V1001_README\"},{\"type\": \"view\",\"name\": \"咪呦社区\",\"url\": \"http://wsq.qq.com/reflow/238370680\"},"
		<<"{\"name\": \"更多\",\"sub_button\": [{\"type\": \"view\",\"name\": \"官网\",\"url\": \"http://www.miyomate.com/m/\"},"
		<<"{\"type\": \"click\",\"name\": \"意见反馈\",\"key\": \"V1001_OPINION\"},"
		<<"{\"type\": \"view\",\"name\": \"活动介绍\",\"url\": \"http://wsq.qq.com/reflow/238370680\"},"
		<<"{\"type\": \"view\",\"name\": \"咪呦下载\",\"url\": \"http://itunes.apple.com/cn/app/wei/id862410865\"}]}]}";
	content = os.str();
	r = spread_logic::HttpComm::WeiXin::PostWeiXinMenu(access_token_,content,result);
	if(!r)
		return ;
	r = spread_logic::ResolveJson::WeiXin::ResolveWeiXinGetMenuState(result);
	if(!r)
		return ;

}

bool WeiXinMgr::Message(const int socket,const packet::HttpPacket& packet){

	packet::HttpPacket pack = packet;
	std::string content;
	bool r = pack.GetAttrib("content",content);
	//用于注册微信接口
/*
	std::string echostr;
	r = pack.GetAttrib("echostr",echostr);
	if(!r)
		return false;
	r =  spread_logic::SomeUtils::SendFull(socket, echostr.c_str(), echostr.length());
	*/


	weixin_base::WXPacket wxpacket;
	wxpacket.SetContent(content);
	WeiXinMessage(socket,wxpacket);

	return true;
}


bool WeiXinMgr::WeiXinMessage(const int socket,weixin_base::WXPacket& packet){

	std::string msgtype;
 	bool r = packet.GetPacketType(msgtype);
 	if (!r)
 		return false;
	if (msgtype=="text")
		OnTestMsg(socket,packet);
	else if(msgtype=="event")
		OnEventMsg(socket,packet);
	return true;
}




void WeiXinMgr::OnTestMsg(const int socket,weixin_base::WXPacket& packet){
	std::string out = "是我是我";
	std::string put_content;
	bool r = false;
	std::string to_user;
	std::string from_user;
	std::string content;
	packet.GetAttrib(ToUserName,to_user);
	packet.GetAttrib(FromUserName,from_user);
	packet.GetAttrib(Content,content);
	PackageTextMsg(from_user,to_user,out,put_content);
	spread_logic::SomeUtils::SendFull(socket, put_content.c_str(), put_content.length());
}

void WeiXinMgr::OnOptinionEvent(const int socket,weixin_base::WXPacket& packet){
	std::string out = "文字框中输入 : yj您的意见，我们便可收到您宝贵的意见";
	std::string put_content;
	bool r = false;
	std::string to_user;
	std::string from_user;
	std::string content;
	packet.GetAttrib(ToUserName,to_user);
	packet.GetAttrib(FromUserName,from_user);
	packet.GetAttrib(Content,content);
	PackageTextMsg(from_user,to_user,out,put_content);
	spread_logic::SomeUtils::SendFull(socket, put_content.c_str(), put_content.length());
}

void WeiXinMgr::OnReadMeEvenet(const int socket,weixin_base::WXPacket& packet){
	std::string put_content;
	bool r = false;
	std::string to_user;
	std::string from_user;
	std::string content;
	std::list<base::WeiXin::GraphicTextInfo> list;
	std::string title = "使用说明";
	std::string desction = "咪呦使用说明";
	std::string picurl = "http://sr.miu.miglab.com/web/pc/botton_1.png";
	std::string url = "http://mp.weixin.qq.com/s?__biz=MzA4MDM1NDExNg==&mid=200051998&idx=1&sn=34843581b9539a23c45ab7e988f20b62#rd";
	base::WeiXin::GraphicTextInfo info(title,desction,picurl,url);
	list.push_back(info);
	packet.GetAttrib(ToUserName,to_user);
	packet.GetAttrib(FromUserName,from_user);
	PacketGraphicTextMsg(from_user,to_user,list,put_content);
	spread_logic::SomeUtils::SendFull(socket, put_content.c_str(), put_content.length());
}

void WeiXinMgr::OnClick(const int socket,weixin_base::WXPacket& packet){
	std::string opcode;
	std::string key = "EventKey";
 	bool r = packet.GetAttrib(key,opcode);
 	if (!r)
 		return ;
 	if(opcode=="V1001_README")
 		OnReadMeEvenet(socket,packet);
 	else if(opcode=="V1001_OPINION")
 		OnOptinionEvent(socket,packet);

}

void WeiXinMgr::OnView(const int socket,weixin_base::WXPacket& packet){
	std::string content = " ";
	spread_logic::SomeUtils::SendFull(socket, content.c_str(), content.length());
}

void WeiXinMgr::OnEventMsg(const int socket,weixin_base::WXPacket& packet){
	std::string event_type;
	std::string key = "Event";
 	bool r = packet.GetAttrib(key,event_type);
 	if (!r)
 		return ;
 	if(event_type=="CLICK")
 		OnClick(socket,packet);
 	else if(event_type=="VIEW")
 		OnView(socket,packet);


}


void WeiXinMgr::PacketGraphicTextMsg(const std::string& to_user,const std::string& from_user,
				std::list<base::WeiXin::GraphicTextInfo>& list,std::string& out_content){

	std::stringstream os;
	os<<"<xml>"
		<<"<ToUserName><![CDATA["<<to_user<<"]]></ToUserName>"
		<<"<FromUserName><![CDATA["<<from_user<<"]]></FromUserName>"
	    <<"<CreateTime>"<<time(NULL)<<"</CreateTime>"
	    <<"<MsgType><![CDATA[news]]></MsgType>"
	    <<"<ArticleCount>"<<list.size()<<"</ArticleCount>"
	    <<"<Articles>";
	while(list.size()>0){
		base::WeiXin::GraphicTextInfo info = list.front();
		list.pop_front();
		os<<"<item><Title><![CDATA["<<info.title()<<"]]></Title>"
			"<Description><![CDATA["<<info.desction()<<"]]></Description>"
			"<PicUrl><![CDATA["<<info.picurl()<<"]]></PicUrl>"
			"<Url><![CDATA["<<info.url()<<"]]></Url>"
			<<"</item>";
	}
	os<<"</Articles></xml>";
	out_content = os.str().c_str();

}
void WeiXinMgr::PackageTextMsg(const std::string& to_user,const std::string& from_user,
				const std::string& content,std::string& out_content){

	std::stringstream os;
	os<<"<xml>"
		<<"<ToUserName><![CDATA["<<to_user<<"]]></ToUserName>"
	    <<"<FromUserName><![CDATA["<<from_user<<"]]></FromUserName>"
	    <<"<CreateTime>"<<time(NULL)<<"</CreateTime>"
	    <<"<MsgType><![CDATA[text]]></MsgType>"
	    <<"<Content><![CDATA["<<content.c_str()<<"]]></Content>"
	    <<"<FuncFlag>0</FuncFlag>"
	    "</xml>";
	out_content = os.str().c_str();
}

}
