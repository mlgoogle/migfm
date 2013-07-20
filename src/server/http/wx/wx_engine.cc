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
// 		if (str[index]==c)
// 			MIG_DEBUG(USER_LEVEL,"========");
// 		else
// 			dest_string.append(1,str[index]);
		if (str[index]!=c)
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
	douban_url_= "http://douban.fm/j/mine/playlist?type=n&sid=&pt=0.0&from=mainsite&channel=";

	//segment_url_ = "http://60.191.222.130";
	segment_url_ = "http://42.121.112.248";

    get_song_url_ = "http://121.199.32.88/getmusicurl.ashx";
	channel_desc_ = base::ConStants::channel_dec();
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

	InitChannelAndMode();
    return r;
}

bool
WXInfoEngine::InitChannelAndMode(void){
	/*channel_.push_back("一号");
	channel_.push_back("二号");
	channel_.push_back("三号");
	channel_.push_back("四号");
	channel_.push_back("五号");
	channel_.push_back("六号");
	channel_.push_back("七号");
	channel_.push_back("八号");
	channel_.push_back("九号");
	channel_.push_back("十号");*/
	base_storage::MysqlSerial::GetChannelInfo(channel_mode_,channel_num_);
	mode_map_["mm"] = "心情模式";
	mode_map_["ms"] = "情景模式";
	mode_map_["mn"] = "普通模式"; 
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
		if (name=="EventKey")
			continue;
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
	/*MIG_DEBUG(USER_LEVEL,"%s",msgtype.c_str());*/
	if (msgtype=="text")
		ProcessMsgText(msg);
	else if (msgtype=="event")
		ProcessEventText(msg);
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
WXInfoEngine::ProcessEventText(WXPacket& msg){
	std::string str_event;
	std::string to_user;
	std::string from_user;
	std::stringstream os;
	std::string  welnews = "欢迎关注";
	msg.GetAttrib(ToUserName,to_user);
	msg.GetAttrib(FromUserName,from_user);
	msg.GetAttrib(Event,str_event);
	std::string content;
	if (str_event=="subscribe"){
		content="MIG音乐助手是以推荐为主的微信应用,而且在移动设备上不需要装任何音乐app,"
		"便可想听你想要的音乐:\n"
		"1,有20个音乐频道,每个音乐频道将为你推荐不同风格的音乐.按发送字符\"n\"后,"
		"系统会推荐一首音乐,提供试听."
		"如果觉得该频道不对味,可发送字符\"cc\",便可切换音乐频道.系统默认为频道一\n\n"
		"2,提供歌曲推荐,歌手推荐.可输入歌曲名.如:如果没有你 "
		"系统便会自动推荐任意一个演绎改歌曲的歌手的音乐."
		"若输入歌手名: 张学友 系统便会自动推荐一首该歌手的任意一首歌曲.\n\n"
		"PS:如果你是苹果用户,那么使用更简便的功能:微信的语音助手插件"
		"(在设置中,找到功能按钮,点击去便可添加)."
		"只需对其说:王菲 便可获取到系统推荐的王菲一首歌,省去了手工输入麻烦.\n"
		"如有问题和建议可微信直接回复该号码,我们即可马上收到."
		"感谢大家内测的支持,下个月将发布正式版,并加入心情推荐和场景推荐\n";
		
		/*char* utf_content = NULL;
		size_t utf_content_size = 0;
		std::string str_utf8_content;*/
		base::BasicUtil::GB2312ToUTF8(content.c_str(),content.length(),&utf_content,&utf_content_size);
		str_utf8_content.assign(utf_content,utf_content_size);
		MIG_DEBUG(USER_LEVEL,"%s",str_utf8_content.c_str());
		/*PackageTextMsg(from_user,to_user,str_utf8_content);
		if (utf_content){
		delete [] utf_content;
		utf_content = NULL;
		}*/
 		std::string temp_msg;
 		os<<"<ToUserName><![CDATA["<<from_user.c_str()<<"]]></ToUserName>"
 		  <<"<FromUserName><![CDATA["<<to_user.c_str()<<"]]></FromUserName>"
 		  <<"<CreateTime>"<<time(NULL)<<"</CreateTime>"
 		  <<"<MsgType><![CDATA["<<welnews.c_str()<<"]]></MsgType>"
 		  <<"<ArticleCount>2</ArticleCount><Articles>";
 		os<<"<item>";
 		HttpMigMusicWebFM(temp_msg);
 		os<<temp_msg.c_str();
 		os<<"</item>";
 		os<<"</Articles><FuncFlag>1</FuncFlag></xml>";
 		content = os.str();
 		MIG_DEBUG(USER_LEVEL,"%s",content.c_str());
 		char* utf_content = NULL;
 		size_t utf_content_size = 0;
 		std::string str_utf8_content;
 		base::BasicUtil::GB2312ToUTF8(content.c_str(),content.length(),
 			                          &utf_content,&utf_content_size);
 		str_utf8_content.assign(utf_content,utf_content_size);
 		content_ = utf_content;
 		if (utf_content){
 			delete [] utf_content;
 			utf_content = NULL;
 		}
	}
}

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

	if (content_s=="n"){
		PullAnyMusicMsg(to_user,from_user);
	}else if (content_s=="cc"){
		ChangeChannel(to_user,from_user);
	}else if (content_s=="jazz"){
		SettingChannel(to_user,from_user,9);
	}else if (content_s=="my"){
		SettingChannel(to_user,from_user,7);
	}else if(content_s=="class"){
                SettingChannel(to_user,from_user,17);
        }else if(content_s=="hy"){
                SettingChannel(to_user,from_user,0);
        }else if(content_s=="rock"){
                SettingChannel(to_user,from_user,6);
        }else if (content_s[0]=='r'){
		RecommendationMusic(to_user,from_user,content_s);
	}else if (content_s=="lp1"||content_s=="lp2"||content_s=="lp3"||
		      content_s=="lp4"||content_s=="lp5"||content_s=="lp6"||
			  content_s=="lp7"||content_s=="lp8"){
		 std::string title;
		 std::string url;
		 std::stringstream os;
		 if (content_s=="lp1"){
			 title = "擦身而过";
			 url = "http://42.121.14.108/wx/lp1.mp3";
		 }else if (content_s=="lp2"){
			 title = "出界";
			 url = "http://42.121.14.108/wx/lp2.mp3";
		 }else if (content_s=="lp3"){
			 title = "倒带";
			 url = "http://42.121.14.108/wx/lp3.mp3";
		 }else if (content_s=="lp4"){
			 title = "解脱";
			 url = "http://42.121.14.108/wx/lp4.mp3";
		 }else if (content_s=="lp5"){
			 title = "天灯";
			 url = "http://42.121.14.108/wx/lp5.mp3";
		 }else if (content_s=="lp6"){
			 title = "雨蝶";
			 url = "http://42.121.14.108/wx/lp6.mp3";
		 }else if (content_s=="lp7"){
			 title = "日不落";
			 url = "http://42.121.14.108/wx/lp7.mp3";
		 }else if (content_s=="lp8"){
			 title = "原来你也在这里";
			 url = "http://42.121.14.108/wx/lp8.mp3";
		 }
		 os<<"某只会唱歌的萝卜";
		 char* r_title = NULL;
		 size_t r_title_size = 0;
		 char* r_os = NULL;
		 size_t r_os_size = 0;
		 base::BasicUtil::GB2312ToUTF8(title.c_str(),title.length(),&r_title,&r_title_size);
		 base::BasicUtil::GB2312ToUTF8(os.str().c_str(),os.str().length(),&r_os,&r_os_size);
		 std::string s_title;
		 std::string s_os;
		 s_title.assign(r_title,r_title_size);
		 s_os.assign(r_os,r_os_size);
	     PackageMusicMsg(from_user,to_user,s_title,s_os,url,url);
		 if (r_title){
			delete [] r_title;
			r_title = NULL;
		 }
		 if (r_os){
			 delete[] r_os;
			 r_os = NULL;
		 }
	}else if (content_s=="mm"||content_s=="ms"||content_s=="mn"){//心情
		SetUserMode(from_user,to_user,content_s);
	}else{
		//获取用户当前状态 心情,mm 场景ms 普通mn
		std::string mode;
		r = GetUserMode(from_user,mode);
		if (r&&(mode=="mm"||mode=="ms")){
			PullMoodAndSenceMusicInfo(to_user,from_user,content_s,mode);
			return ;
		}

		r = SegmentWordMsg(to_user,from_user,content_s);
		if (!r)
			PullAnyMusicMsg(to_user,from_user);
// 		if (!r)
// 			PullRobotTextMsg(to_user,from_user,content_s);
	}

}

bool WXInfoEngine::SetUserMode(std::string& from_user,std::string to_user,
							   std::string& mode){
   std::stringstream key;
   std::stringstream content;
   bool r = false;
   key<<from_user.c_str()
	   <<"_m";

   MIG_DEBUG(USER_LEVEL,"key[%s]",key.str().c_str());
   r = base_storage::MemDicSerial::SetString(key.str().c_str(),key.str().length(),
	                                         mode.c_str(),mode.length());

   std::map<std::string,std::string>::iterator it = mode_map_.find(mode);
   content<<"切换模式成功，您已经切换到"<<it->second.c_str();
   size_t out_len;
   char* out;
   std::string str_out;
   base::BasicUtil::GB2312ToUTF8(content.str().c_str(),
	                             content.str().length(),
								 &out,
								 &out_len);
   str_out.assign(out,out_len);
   if (out){
	   delete [] out;
	   out = NULL;
   }
   PackageTextMsg(from_user,to_user,str_out);
   return r;
}

bool 
WXInfoEngine::GetUserMode(const std::string& from_user,std::string& mode){
	std::stringstream key;
	char* value = NULL;
	size_t value_len = 0;
	bool r = false;
	key<<from_user.c_str()
	   <<"_m";

	r = base_storage::MemDicSerial::GetString(key.str().c_str(),
		                                      key.str().length(),
											  &value,&value_len);

	mode.assign(value,value_len);
	if (value){
		delete [] value;
		value = NULL;
	}
	return r;
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
WXInfoEngine::GetMusicWordInfo(std::list<std::string>& word_list, 
							   std::string& word_name){
	 
	int32 list_num = word_list.size();
	if (list_num==0)
		return false;
	word_name = base64_encode((unsigned char*)(word_list.front().c_str()),word_list.front().length());
	word_list.pop_front();
	return true;
}

bool 
WXInfoEngine::PutRadomSong(base::MusicInfo& mi,
						   std::list<std::string> mt_list, 
						   std::string& content,std::string& flag){
   
   bool r =false;
   for (std::list<std::string>::iterator it = mt_list.begin();
	   it!=mt_list.end();it++){
		   std::stringstream os;
		   std::string song_id;
		   std::string music_info;
		   std::string content_url;
		   std::stringstream dec_os;
		   base::MusicInfo smi;
		   std::string base64_title = base64_encode((unsigned char*)((*it).c_str()),
											(*it).length());
		   os<<flag<<base64_title;
		   r = base_storage::RedisDicSerial::GetMusicMapRadom(os.str(),song_id);
		   if (!r)
			   continue;
		   r = base_storage::RedisDicSerial::GetMusicInfos(song_id,music_info);
		   if (!r)
			   continue;
		   r = smi.UnserializedJson(music_info);
		   if (!r)
			   continue;
		   r = wx_get_song_->GetSongInfo(smi.artist(),smi.title(),
										smi.album_title(),content_url);
		   if (!r)
			   continue;
		   smi.set_url(content_url);
		   dec_os<<base64_decode(smi.artist()).c_str()<<" "
			   <<base64_decode(smi.album_title()).c_str()
			   <<" "<<smi.pub_time();
		   content = dec_os.str();
		   mi = smi;
		   return true;
   }
   return false;
}

bool
WXInfoEngine::PutDesignationMusicInfo(base::MusicInfo& mi,std::list<std::string> mt_list, 
									  std::string& artist,std::string& content){
	
	std::stringstream os;
	os<<"ad_"<<artist;
	bool r = false;
	for (std::list<std::string>::iterator it = mt_list.begin();
		it!=mt_list.end();it++){
			std::string song_id;
			std::string music_info;
			std::string content_url;
			std::stringstream dec_os;
			std::string base64_title = base64_encode((unsigned char*)((*it).c_str()),(*it).length());
			r = base_storage::RedisDicSerial::GetMusicMapInfo(os.str(),
											  base64_title,song_id);
			if (!r)
				continue;
			r = base_storage::RedisDicSerial::GetMusicInfos(song_id,music_info);
			if (!r)
				continue;
			r = mi.UnserializedJson(music_info);
			if (!r)
				continue;
			r = wx_get_song_->GetSongInfo(mi.artist(),mi.title(),
				                          mi.album_title(),content_url);
			if (!r)
				continue;
			mi.set_url(content_url);
			dec_os<<base64_decode(mi.artist()).c_str()<<" "
			  <<base64_decode(mi.album_title()).c_str()
				<<" "<<mi.pub_time();
			content = dec_os.str();
			break;
	}
	return  r;
}

//心情词性：mm 
//场景词性: ms

bool
WXInfoEngine::GetMoodAndScenesMusicInfos(base::MusicInfo& mi,std::string& content,
										 std::string& word_flag){
	bool r = false;
	std::string word_mi = "mi";
	std::string ms_word;
	std::string ms_word_id;
	std::stringstream os;
	std::map<std::string,std::list<std::string> >::iterator it 
		= word_map_.find(word_flag);
	
	if (it==word_map_.end()){
		return false;
	}
	r = GetMusicWordInfo(it->second,ms_word);
	if (!r)
		return false;

	//获取心情,场景词对于的心情，场景号
	r = base_storage::RedisDicSerial::GetMoodAndScensId(ms_word,ms_word_id);
	if (!r)
		return false;
	//获取歌手
	std::map<std::string,std::list<std::string> >::iterator itr
		= word_map_.find(word_mi);
	
	while (itr!=word_map_.end()){//指定推荐
		//mapname:mmd_1
		//获取改歌手该心情场景有多少歌
		//key:base64(astist)_wordflag
		std::stringstream os_artist;
		std::stringstream os_word_mapname;
		std::string artist;
		std::string artist_num;
		std::string base64_artist;
		r = GetMusicWordInfo(itr->second,artist);
		if (!r)
			break;
// 		base64_artist = base64_encode((unsigned char*)(artist.c_str()),
// 			                           artist.length());
		os_artist<<artist.c_str()
			     <<"_"
			     <<word_flag
				 <<ms_word_id;
		r = base_storage::RedisDicSerial::GetArtistMoodAndScensNum(os_artist.str(),
			                                                       artist_num);
		if (!r)
			break;
		int srand_num = (time(NULL)%(atol(artist_num.c_str())));
		os_word_mapname<<word_flag<<"_d"<<ms_word_id;
		std::string song_id;
		std::stringstream key;
		key<<artist.c_str()<<"_"<<srand_num;

		MIG_DEBUG(USER_LEVEL,"mapname[%s] key [%s]",os_word_mapname.str().c_str(),
			                                        key.str().c_str());
		r = base_storage::RedisDicSerial::GetMusicMapInfo(os_word_mapname.str(),
			                                              key.str(),song_id);
		if (!r)
			break;
		GetOneMusicInfo(song_id,mi,content);
		return true;

	}
	//mapname:mmr_1
	os<<word_flag<<"_r"<<ms_word_id;
	MIG_DEBUG(USER_LEVEL,"map_name:[%s]",os.str().c_str());
	std::string song_id;
	r = base_storage::RedisDicSerial::GetMusicMapRadom(os.str(),song_id);

	if (!r)
		return false;
	GetOneMusicInfo(song_id,mi,content);
	return true;
	
}

bool
WXInfoEngine::GetOneMusicInfo(const std::string& song_id, 
							  base::MusicInfo& mi, 
							  std::string& content){
    
	std::stringstream dec_os;
	base::MusicInfo smi;
	std::string music_info;
	std::string content_url;
	bool r = false;
	r = base_storage::RedisDicSerial::GetMusicInfos(song_id,music_info);
	if (!r)
		return false;
	MIG_DEBUG(USER_LEVEL,"song_id[%s] json[%s]",song_id.c_str(),
		      music_info.c_str());

	r = smi.UnserializedJson(music_info);
	if (!r)
		return false;
	MIG_DEBUG(USER_LEVEL,"artist[%s] title[%s]",smi.artist().c_str(),
		smi.title().c_str());
	r = wx_get_song_->GetSongInfo(smi.artist(),smi.title(),
	smi.album_title(),content_url);
	if (!r)
		return false;
	smi.set_url(content_url);
	dec_os<<base64_decode(smi.artist()).c_str()<<" "
	<<base64_decode(smi.album_title()).c_str()
	<<" "<<smi.pub_time();
	content = dec_os.str();
	mi = smi;
	return true;
}

bool 
WXInfoEngine::GetMusicInfos(base::MusicInfo& mi,std::string& content){
	bool r = false;
	std::string word_mi = "mi";
	std::string word_mt = "mt";
	std::string word_sr = "sr_";
	std::string word_ar = "ar_";
	std::string base64_artist;
	std::string base64_title;
	std::map<std::string,std::list<std::string> >::iterator it 
		= word_map_.find(word_mi);
	
	std::map<std::string,std::list<std::string> >::iterator itr 
		= word_map_.find(word_mt);
	//歌手+歌曲
	if (it!=word_map_.end()&&itr!=word_map_.end()){
		GetMusicWordInfo(it->second,base64_artist);
		r  = PutDesignationMusicInfo(mi,itr->second,base64_artist,content);
	}
	if (r)
		return r;

	//歌曲
	if (itr!=word_map_.end())
		r = PutRadomSong(mi,itr->second,content,word_sr);
	if (r)
		return r;

	//歌手
	if (it!=word_map_.end())
		r = PutRadomSong(mi,it->second,content,word_ar);
	if (r)
		return r;

	return r;
}

bool 
WXInfoEngine::PullMoodAndSenceMusicInfo(std::string& to_user,
								std::string& from_user, 
								std::string& content_s,
								std::string& word_flag){
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

	r = GetMoodAndScenesMusicInfos(mi,content,word_flag);
	if (!r)
		return false;

	//MIG_DEBUG(USER_LEVEL,"title[%s]",mi.title().c_str());
	PackageMusicMsg(from_user,to_user,base64_decode(mi.title()),
		            content,mi.url(),mi.url());
	word_map_.clear();
	return true;
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
	 if (r){
		PackageMusicMsg(from_user,to_user,base64_decode(mi.title()),
		 decs,mi.url(),mi.url());
		return true;
	 }
	 word_map_.clear();
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
WXInfoEngine::RecommendationMusic(std::string& to_user,
								  std::string& from_user, 
								  std::string& content){


    //char index = content[1];
	std::string id;
	id.assign(content.c_str()+1,content.length()-1);
	std::string title;
	std::string decs;
	std::string base64_title;
	std::string base64_decs;
	std::string url;
	bool r =false;
	r = base_storage::MysqlSerial::GetWXMusicInfo(id,base64_title,
		                                         base64_decs,url);
	if (r){
		title = base64_decode(base64_title);
		decs = base64_decode(base64_decs);
		PackageMusicMsg(from_user,to_user,title,decs,url,url);

	}
	/*std::string url;
	std::string title;
	std::string decs;
	std::string utf_8_title;
	std::string utf8_decs;
	char* str_title = NULL;
	size_t str_title_size = 0;
	char* str_decs =NULL;
	size_t str_decs_size = 0;
    
	if (content=="r1"){
		return ;
	}else if (content=="r2"){
		title = "孤独的人是可耻的";
		decs = "张楚 1994 孤独的人是可耻的";
		url = "http://42.121.14.108/wx/r2.mp3";
	}else if (content=="r3"){
		title = "一块红布";
		decs = "崔健 1991 解决";
		url = "http://42.121.14.108/wx/r3.mp3";
	}else if (content=="r4"){
		title = "花太香";
		decs = "任贤齐 2001 飞鸟";
		url = "http://42.121.14.108/wx/r4.mp3";
	}else if (content=="r5"){
		title = "鲁冰花";
		decs = "甄妮 2003 鲁冰花";
		url = "http://42.121.14.108/wx/r5.mp3";
	}
	base::BasicUtil::GB2312ToUTF8(title.c_str(),title.length(),&str_title,&str_title_size);
	utf_8_title.assign(str_title,str_title_size);
	base::BasicUtil::GB2312ToUTF8(decs.c_str(),decs.length(),&str_decs,&str_decs_size);
	utf8_decs.assign(str_decs,str_decs_size);
	PackageMusicMsg(from_user,to_user,utf_8_title,utf8_decs,url,url);
	if (str_decs){
		delete [] str_decs;
		str_decs = NULL;
	}
	if (str_title){
		delete [] str_title;
		str_title = NULL;
	}*/
}

void 
WXInfoEngine::SettingChannel(std::string& to_user,std::string& from_user,
							 int new_channel){
	bool r = false;
	std::string  content;
	std::stringstream  msg;
	int32 channel = 0;
	base::ChannelInfo channel_info;

	channel_info = channel_mode_[new_channel];

	r = HttpGetDoubanMusicInfo(content,
		atol(channel_info.douban_index().c_str()));

	if (r){
		time_t json_time = time(NULL)+(60*60*2);
		r = SetMemMusicInfo(from_user,1,new_channel,content,json_time);
		msg<<channel_desc_.c_str()
			<<channel_info.channel_name().c_str();

		MIG_DEBUG(USER_LEVEL,"channel_name[%s] index[%s]",
			channel_info.channel_name().c_str(),
			channel_info.douban_index().c_str());

		std::string msg_content = msg.str();
		MIG_DEBUG(USER_LEVEL,"%s",msg_content.c_str());

		PackageTextMsg(from_user,to_user,msg_content);
	}
	else{
		MIG_ERROR(USER_LEVEL,"GetDoubanMusic Error");
		return ;
	}
}

void
WXInfoEngine::ChangeChannel(std::string& to_user,std::string& from_user){
	bool r = false;
	std::string  content;
	std::stringstream  msg;
	int32 channel = 0;
	base::ChannelInfo channel_info;
	r = GetMemChanncel(from_user,channel);
	int32 new_channel = (channel+1)%channel_num_;
	MIG_INFO(USER_LEVEL,"new_channel[%d]",new_channel);

	channel_info = channel_mode_[new_channel];

	r = HttpGetDoubanMusicInfo(content,
		                       atol(channel_info.douban_index().c_str()));

	if (r){
		time_t json_time = time(NULL)+(60*60*2);
		r = SetMemMusicInfo(from_user,1,new_channel,content,json_time);
		msg<<channel_desc_.c_str()
			<<channel_info.channel_name().c_str();

		MIG_DEBUG(USER_LEVEL,"channel_name[%s] index[%s]",
			channel_info.channel_name().c_str(),
			channel_info.douban_index().c_str());

		std::string msg_content = msg.str();
		MIG_DEBUG(USER_LEVEL,"%s",msg_content.c_str());

		PackageTextMsg(from_user,to_user,msg_content);
	}
	else{
		MIG_ERROR(USER_LEVEL,"GetDoubanMusic Error");
		return ;
	}

}

void 
WXInfoEngine::PullAnyMusicMsg(std::string& to_user,std::string& from_user){

	std::string douban_url;
	std::string durl;
	std::string title;
	std::string decs;
	std::string content;
	int channel = 0;
	bool r = false;
	time_t json_time;
	time_t current_time = time(NULL);
	base::ChannelInfo ci;

	//channcel|num|time|json
	r = GetMemMusicInfo(from_user,durl,title,decs,json_time,channel);

	ci = channel_mode_[channel];
	//判断是否存在或者是否超时
	if ((!r)||(json_time<current_time)){
		r = HttpGetDoubanMusicInfo(content,atol(ci.douban_index().c_str()));
		if (r){
			time_t json_time = time(NULL)+(60*60*2);
		    r = SetMemMusicInfo(from_user,1,channel,content,json_time);
			r = ParseJson(0,content,from_user,durl,title,decs);
		}
		else{
			MIG_ERROR(USER_LEVEL,"GetDoubanMusic Error");
			return ;
		}
	}

	PackageMusicMsg(from_user,to_user,title,decs,durl,durl);

	/*std::string content;
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
	count_++;*/
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

bool WXInfoEngine::GetMemChanncel(std::string &from_user, int &current_channel){
	bool r = false;
	size_t value_len = 0;
	char* value = NULL;
	int32 num = 0;
	/*std::string content;*/
	r = base_storage::MemDicSerial::GetString(from_user.c_str(),
		from_user.length(),&value,&value_len);
	if (!r)
		return false;

	const char* temp_value = value;
	char* temp = strstr(temp_value,"|");
	int channel_pos = temp  - temp_value;
	std::string channel;
	channel.assign(temp_value,channel_pos);
	current_channel = atol(channel.c_str());

	/*temp_value = temp+1;
	temp = strstr(temp_value,"|");
	int num_pos = temp - temp_value;
	std::string num_str;
	num_str.assign(temp_value,num_pos);
	num = atol(num_str.c_str());

	temp_value = temp+1;
	temp = strstr(temp_value,"|");
	int time_pos = temp - temp_value;
	std::string current_time;
	current_time.assign(temp_value,time_pos);*/
	//current = atol(current_time.c_str());
	//content.assign(temp+1,value_len-4);
	//MIG_DEBUG(USER_LEVEL,"content %s",content.c_str());
}

bool WXInfoEngine::GetMemMusicInfo(std::string& from_user,
								   std::string& durl,std::string& title,
								   std::string& decs,time_t& current,
								   int& current_channel){
	 bool r = false;
	 size_t value_len = 0;
	 char* value = NULL;
	 int num = 0;
	 std::string content;
	 base::ChannelInfo ci;
	 r = base_storage::MemDicSerial::GetString(from_user.c_str(),
	                             from_user.length(),&value,&value_len);
	 if (!r)
		 return false;

	 const char* temp_value = value;
	 char* temp = strstr(temp_value,"|");
	 int channel_pos = temp  - temp_value;
	 std::string channel;
	 channel.assign(temp_value,channel_pos);
	 current_channel = atol(channel.c_str());

	 temp_value = temp+1;
	 temp = strstr(temp_value,"|");
	 int num_pos = temp - temp_value;
	 std::string num_str;
	 num_str.assign(temp_value,num_pos);
	 num = atol(num_str.c_str());

	 temp_value = temp+1;
	 temp = strstr(temp_value,"|");
	 int time_pos = temp - temp_value;
	 std::string current_time;
	 current_time.assign(temp_value,time_pos);
	 current = atol(current_time.c_str());

	 content.assign(temp+1,value_len-4);
	 MIG_DEBUG(USER_LEVEL,"content %s",content.c_str());
	 r = ParseJson(num,content,from_user,durl,title,decs);

	 //是否到达最后一个
	 if((num+1)<5)
		 r = SetMemMusicInfo(from_user,(num+1),atol(channel.c_str()),content,current);
	 else{
		 ci = channel_mode_[current_channel];
		 std::string s_content;
		 r = HttpGetDoubanMusicInfo(s_content,atol(ci.douban_index().c_str()));
		 if (r){
			 time_t json_time = time(NULL)+(60*60*2);
			 r = SetMemMusicInfo(from_user,0,current_channel,s_content,json_time);
			 r = ParseJson(0,content,from_user,durl,title,decs);
		 }
		 else{
			 MIG_ERROR(USER_LEVEL,"GetDoubanMusic Error");
			 return r;
		 }
	 }
	 
	 return true;
}

bool WXInfoEngine::SetMemMusicInfo(std::string& from_user,
								   int32 num,int32 channel, 
								   std::string& json,time_t json_time){
    std::stringstream os;
	bool r = false;
	os<<channel<<"|"<<num<<"|"<<json_time<<"|"<<json;
	r = base_storage::MemDicSerial::SetString(from_user.c_str(),from_user.length(),
		                                      os.str().c_str(),os.str().length());

	return r;
}

bool WXInfoEngine::ParseJson(int32 num,std::string& content,
							 std::string& from_user, 
							 std::string& durl,std::string& title,
							 std::string& decs){

	 Json::Reader reader;
	 Json::Value  root;
	 std::string artist;
	 std::string pub_time;
	 std::string albumtitle;
	 int32 music_infos_size;
	 std::stringstream os;
	 Json::Value song;
	 bool r = false;
	 r = reader.parse(content.c_str(),root);
	 if (!r){
		 MIG_ERROR(USER_LEVEL,"parse json error[%s]",content.c_str());
		 return false;
	 }
	 music_infos_size = root["song"].size();
	 if (music_infos_size<=0){
		 MIG_ERROR(USER_LEVEL,"song valiled size[%d]",music_infos_size_);
		 return false;
	 }
	 song = root["song"];
	 if (num<0||num>music_infos_size){
		 MIG_ERROR(USER_LEVEL,"num error [%d]",num);
		 return false;
	 }

	 artist = song[num]["artist"].asString();
	 durl = song[num]["url"].asString();
	 title = song[num]["title"].asString();
	 pub_time = song[num]["public_time"].asString();
	 albumtitle = song[num]["albumtitle"].asString();

	 MIG_DEBUG(USER_LEVEL,
		       "artist[%s] durl[%s] title[%s] pubtime[%s] albuntitle[%s]",
			   artist.c_str(),durl.c_str(),title.c_str(),pub_time.c_str(),
			   albumtitle.c_str());
	 os<<artist.c_str()<<" "<<albumtitle.c_str()<<" "<<pub_time.c_str();
	 decs = os.str();
	 return true;
}

bool
WXInfoEngine::HttpGetDoubanMusicInfo(std::string& content,int32 channel){
	std::string douban_url;
	douban_url = douban_url_ ;
	bool r = false;
	std::stringstream os;
	os<<channel;
	douban_url.append(os.str().c_str());
	MIG_DEBUG(USER_LEVEL,"doubam_url[%s]",douban_url.c_str());
	wxinfo::HttpResponse http(douban_url);
	r = http.Get();
	if (!r){
		MIG_ERROR(USER_LEVEL,"get douban error");
		return false;
	}
	r = http.GetContent(content);
	return r;
}

void WXInfoEngine::HttpMigMusicWebFM(std::string& msg){
	std::stringstream os;
	std::string title = "Test";
	std::string description = "test descr";
	std::string pic = "http://42.121.14.108/wx/1.jpg";
	std::string url = "http://mp.weixin.qq.com/mp/appmsg/show?__biz=MjM5ODgzNTUyMQ==&appmsgid=10000171&itemidx=1&sign=3f906a66b1655dc8d83227da39b37eee#wechat_redirect";

	os<<"<Title><![CDATA["<<title.c_str()<<"]]></Title>"
		<<"<Description><![CDATA["
		<<description.c_str()<<"]]></Description>"
		<<"<PicUrl><![CDATA["<<pic.c_str()<<"]]></PicUrl>"
		<<"<Url><![CDATA["<<url.c_str()<<"]]></Url>";
	msg = os.str();
}

}
