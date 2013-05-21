#include "wx_engine.h"
#include "log/mig_log.h"
#include "config/config.h"
#include "storage/db_serialization.h"
#include "storage/dic_serialization.h"
#include "basic/basic_util.h"
#include "xmpp/xmlelement.h"
#include "basic/constants.h"
#include "basic/base64.h"
#include "http_response.h"
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
,stanzaParser_(&stanzaParseHandler_)
,count_(0)
,music_infos_size_(0){
	std::string head;


	head = "<stream:stream from=\"gmail.com\" " 
		"id=\"1F83A90940271513\" "  //通过SSO获取 
		"version=\"1.0\" " 
		"xmlns:stream=\"http://etherx.jabber.org/streams\" " 
		"xmlns=\"jabber:client\">";
	stanzaParser_.Parse(head.c_str(),head.length(),false);


	url_ = "http://sandbox.api.simsimi.com/request.p?key=274d7567-e7d8-4d08-a8b2-5c5a86721846&lc=ch&ft=1.0&text=";
	douban_url_= "http://douban.fm/j/mine/playlist?type=n&sid=&pt=0.0&from=mainsite&channel=1";

	//segment_url_ = "http://60.191.222.130";
	segment_url_ = "http://42.121.112.248";

    get_song_url_ = "http://121.199.32.88/GetMusicUrl.ashx";
}



WXInfoEngine::~WXInfoEngine(){
	if (wx_get_song_!=NULL){
		delete wx_get_song_;
		wx_get_song_ = NULL;
	}
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
	std::string  content;
	Json::Reader reader;
	Json::Value  root;
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

	wx_get_song_ = wxinfo::WXGetSongUrl::Create(wxinfo::TYPE_BAIDU);
	if (wx_get_song_==NULL){
		MIG_ERROR(USER_LEVEL,"wx_get_song create error");
		return r;
	}
	wx_get_song_->Init(get_song_url_);

	wxinfo::HttpResponse http(douban_url_);
	http.Get();
	http.GetContent(content);

	r = reader.parse(content.c_str(),root);
	if (!r){
		MIG_ERROR(USER_LEVEL,"parse json error[%s]",content.c_str());
		return r;
	}
	music_infos_size_ = root["song"].size();
	if (music_infos_size_<=0){
		MIG_ERROR(USER_LEVEL,"song valiled size[%d]",music_infos_size_);
		return r;
	}
	song_ = root["song"];
	MIG_DEBUG(USER_LEVEL,"%s",content.c_str());
    return r;
}

bool 
WXInfoEngine::GetMusicInfosFromStroage(){
	std::list<base::MusicInfo> music_list;
	return base_storage::MysqlSerial::GetMusicAll(music_list);
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
		MIG_DEBUG(USER_LEVEL,"name[%s] name_content[%s]",name.c_str(),name_content.c_str());
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
	MIG_DEBUG(USER_LEVEL,"%s",msgtype.c_str());
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
	bool r = false;
	msg.GetAttrib(ToUserName,to_user);
	msg.GetAttrib(FromUserName,from_user);
	msg.GetAttrib(Content,content_s);

	if (content_s=="1"){
		PullAnyMusicMsg(to_user,from_user);
	}else{
		r = SegmentWordMsg(to_user,from_user,content_s);
		if (!r)
			PullRobotTextMsg(to_user,from_user,content_s);
	}


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
// 	std::string title = "指南针--选择坚强";
// 	std::string description = "选择坚强";
// 	std::string s_title;
// 	std::string s_description;
// 	char*  title_output;
// 	size_t title_output_len;
// 
// 	char*  description_output;
// 	size_t description_output_len;
// 
// 	base::BasicUtil::GB2312ToUTF8(title.c_str(),title.length(),
// 									&title_output,&title_output_len);
// 	s_title.assign(title_output,title_output_len);
// 
// 
// 	base::BasicUtil::GB2312ToUTF8(description.c_str(),description.length(),
// 		&description_output,&description_output_len);
// 	s_description.assign(description_output,description_output_len);
// 
// 	std::string url = "http://mr4.douban.com/201305060009/d191e6bc3ef2e322b6af6b4b2edd2149/view/song/small/p1121382.mp3";
	//PackageMusicMsg(from_user,to_user,s_title,s_description,url,url);

}

bool
WXInfoEngine::GetOneMusicInfo(base::MusicInfo& mi,std::string& flag,std::string& key,
						   std::string& content){
	bool r = false;
	std::stringstream sql;
	std::stringstream os;
	Json::Reader reader;
	Json::Value  root;
	std::string content_url;
	sql<<flag.c_str()<<"=\'"<<base::BasicUtil::GetRawString(key).c_str()<<"\' ";
	std::string sql_s =sql.str();
	r = base_storage::MysqlSerial::GetMusicRawDouBan(mi,sql_s);

	os<<base64_decode(mi.artist()).c_str()<<" "<<base64_decode(mi.album_title()).c_str()
		<<" "<<mi.pub_time();
	r = wx_get_song_->GetSongInfo(mi.artist(),mi.title(),mi.album_title(),content_url);
	if (r){
		mi.set_url(content_url);
		MIG_DEBUG(USER_LEVEL,"url[%s]",content_url.c_str());
		content = os.str();
	}
	return r;
}


bool
WXInfoEngine::GetMusicInfo(base::MusicInfo& mi,std::string& content,std::string& key){
	std::stringstream os;
	bool r = false;
	std::string artist = "artist";
	std::string title = "title";
	std::string albumtitle = "albumtitle";
	std::map<std::string,std::list<std::string> >::iterator it = word_map_.find(key);
	if (it==word_map_.end())
		return false;
	for (std::list<std::string>::iterator itr = it->second.begin();
		itr!=it->second.end();itr++){
		//sql<<"artist=\'"<<base64_encode((*itr).c_str()).c_str()<<"\' ";
		r = GetOneMusicInfo(mi,artist,(*itr),content);
		if (r){
			return r;
		}
		r = GetOneMusicInfo(mi,title,(*itr),content);
		if (r){
			return r;
		}
		r = GetOneMusicInfo(mi,albumtitle,(*itr),content);
		if (r){
			return r;
		}
	}
	MIG_DEBUG(USER_LEVEL,"==================\n\n");
	return r;
}

bool 
WXInfoEngine::GetMusicInfos(base::MusicInfo& mi,std::string& content){
	bool r = false;
	std::string nr = "nr";
	std::string n = "n";
	r = GetMusicInfo(mi,content,nr);
	if (r)
		return r;
	r = GetMusicInfo(mi,content,n);
	if (r)
		return r;

	return r;
}

bool
WXInfoEngine::SegmentWordMsg(std::string &to_user, std::string &from_user, 
							 std::string &content_s){

	 wxinfo::HttpPost http(segment_url_);
	 bool r = false;
	 base::MusicInfo mi;
	 std::string content;
	 std::string decs;
	 int port =8080;
	 r = http.Post(content_s.c_str(),port);
	 if (!r){
		 MIG_ERROR(USER_LEVEL,"segment http post error");
		 return false;
	 }

	 http.GetContent(content);
	 MIG_DEBUG(USER_LEVEL,"%s",content.c_str());
	 r = StorageOneWords(content);
	 if (!r){
		 MIG_ERROR(USER_LEVEL,"storage word error");
		 return false;
	 }
	 //StorageWordsDump();
	 r = GetMusicInfos(mi,decs);
	 std::string sjson;
	 std::string mjson;
	 mi.SerializedJson(sjson);
	 base_storage::RedisDicSerial::SetMusicInfos(mi.sid(),sjson);
 //	 r = base_storage::RedisDicSerial::GetMusicInfos(mi.sid(),mjson);

	 if (r){
		PackageMusicMsg(from_user,to_user,base64_decode(mi.title()),
		 decs,mi.url(),mi.url());
		return true;
	 }
	 return false;
}

void
WXInfoEngine::PullRobotTextMsg(std::string& to_user,std::string& from_user,
							   std::string& content_s){
	std::stringstream os;
	std::string  content;
	os<<url_<<content_s;
	Json::Reader reader;
	Json::Value  root;
	std::string url = os.str();
	wxinfo::HttpResponse http(url);
	http.Get();
	http.GetContent(content);

	bool r = reader.parse(content.c_str(),root);
	if (!r){
		MIG_ERROR(USER_LEVEL,"parse json error[%s]",content.c_str());
		return ;
	}

	int ack_code = root["result"].asInt();
	if (ack_code==100){
		std::string reponse = root["response"].asString();
		PackageTextMsg(from_user,to_user,reponse);
	}
}

void 
WXInfoEngine::PullAnyMusicMsg(std::string& to_user,std::string& from_user){
	std::string content;
	Json::Reader reader;
	Json::Value  root;
	bool r = false;
	std::stringstream os;
	std::string s_title;
	std::string s_description;
 	char*  title_output;
 	size_t title_output_len;
 	char*  description_output;
 	size_t description_output_len;
	std::stringstream os_url;
// 	int32 rand_num = 10%(rand());
// 	os_url<<douban_url_<<rand_num;
// 	std::string temp = os_url.str();

	if (count_>=music_infos_size_){
		count_ = 0;
		wxinfo::HttpResponse http(douban_url_);
		http.Get();
		http.GetContent(content);

		r = reader.parse(content.c_str(),root);
		if (!r){
			MIG_ERROR(USER_LEVEL,"parse json error[%s]",content.c_str());
			return ;
		}
		music_infos_size_ = root["song"].size();
		if (music_infos_size_<=0){
			MIG_ERROR(USER_LEVEL,"song valiled size[%d]",music_infos_size_);
			return ;
		}
		song_ = root["song"];
		MIG_DEBUG(USER_LEVEL,"%s",content.c_str());

	}

	//int i = (music_infos_size-1)%(rand());
	MIG_DEBUG(USER_LEVEL,"i [%d]",count_);
	std::string artist = song_[count_]["artist"].asString();
	std::string durl = song_[count_]["url"].asString();
	std::string company = song_[count_]["company"].asString();
	std::string title = song_[count_]["title"].asString();
	std::string public_time = song_[count_]["public_time"].asString();
	std::string albumtitle = song_[count_]["albumtitle"].asString();


	MIG_DEBUG(USER_LEVEL,"artlist[%s] company[%s] titile[%s] albumtitle[%s]",
				artist.c_str(),company.c_str(),title.c_str(),albumtitle.c_str());

	os<<artist.c_str()<<" "<<albumtitle.c_str()<<" "<<public_time.c_str();

	std::string decs = os.str();

	PackageMusicMsg(from_user,to_user,title,decs,durl,durl);
	count_++;
}

void
WXInfoEngine::PackageTextMsg(std::string& to_user,std::string& from_user, 
							 std::string& content){
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
/*<xml>
<ToUserName><![CDATA[toUser]]></ToUserName>
<FromUserName><![CDATA[fromUser]]></FromUserName>
<CreateTime>12345678</CreateTime>
<MsgType><![CDATA[music]]></MsgType>
<Music>
<Title><![CDATA[TITLE]]></Title>
<Description><![CDATA[DESCRIPTION]]></Description>
<MusicUrl><![CDATA[MUSIC_Url]]></MusicUrl>
<HQMusicUrl><![CDATA[HQ_MUSIC_Url]]></HQMusicUrl>
</Music>
<FuncFlag>0</FuncFlag>
</xml>*/
void
WXInfoEngine::PackageMusicMsg(const std::string& to_user,
							  const std::string& from_user,
							  const std::string& title,
							  const std::string& description,
							  const std::string& url, const std::string& hq_url){
	std::stringstream os;
	os<<"<xml>"
		<<"<ToUserName><![CDATA["<<to_user.c_str()<<"]]></ToUserName>"
		<<"<FromUserName><![CDATA["<<from_user.c_str()<<"]]></FromUserName>"
		<<"<CreateTime>"<<time(NULL)<<"</CreateTime>"
		"<MsgType><![CDATA[music]]></MsgType>"
		"<Music>"
		"<Title><![CDATA["<<title.c_str()<<"]]></Title>"
		"<Description><![CDATA["<<description.c_str()<<"]]></Description>"
		"<MusicUrl><![CDATA["<<url<<"]]></MusicUrl>"
		"<HQMusicUrl><![CDATA["<<hq_url<<"]]></HQMusicUrl>"
		"</Music>"
		"<FuncFlag>0</FuncFlag>"
		"</xml>";
	MIG_DEBUG(USER_LEVEL,"%s",os.str().c_str());
	content_ = os.str().c_str();
	word_map_.clear();
}

bool
WXInfoEngine::StorageOneWords(std::string &reponse_msg){
	size_t start_npos;
	size_t end_npos;
	std::string str = reponse_msg;
	while (str.length()!=0){
		start_npos = str.find("&");
		if (start_npos==std::string::npos){
			return false;
		}
		end_npos = str.find("|");
		if (end_npos==std::string::npos){
			return false;
		}
		std::string r = str.substr(0,start_npos);
		std::string n = str.substr(start_npos+1,end_npos-start_npos-1);
		str = str.substr(end_npos+1,str.length());
		StorageOneWord(r,n);
	}
	return true;
}

void WXInfoEngine::StorageOneWord(std::string &attr, std::string &value){
	std::map<std::string,std::list<std::string> >::iterator it =
		word_map_.find(attr);
	if (it!=word_map_.end()){
		it->second.push_back(value);
	}else{
		std::list<std::string> one_word_list;
		one_word_list.push_back(value);
		word_map_[attr] = one_word_list;
	}
}

void WXInfoEngine::StorageWordsDump(){
	for(std::map<std::string,std::list<std::string> >::iterator it =
		word_map_.begin();it!=word_map_.end();it++){
			for (std::list<std::string>::iterator itr = it->second.begin();
				itr != it->second.end();itr++){
					MIG_DEBUG(USER_LEVEL,"attr[%s] name[%s]\n",
						it->first.c_str(),(*itr).c_str());
			}
	}
}

}
