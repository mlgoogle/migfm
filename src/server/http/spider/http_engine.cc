#include "http_engine.h"
#include "config/config.h"
#include "http_response.h"
#include "json/json.h"
#include "storage/db_serialization.h"
#include "basic/basic_util.h"
namespace spider{
	
SpiderHttpEngineImpl::SpiderHttpEngineImpl()
:http_(NULL){
    Init();
}

SpiderHttpEngineImpl::~SpiderHttpEngineImpl(){
    if(http_){delete http_;http_= NULL;}
}

int32 SpiderHttpEngineImpl::Init(){

    bool r = false;
    return 1;
}

int32 SpiderHttpEngineImpl::RequestHttp(const std::string& url){
    std::string content;
    Json::Reader reader;
    Json::Value root;
    std::string sid;
    std::string ssid;
    std::string title;
    std::string album;
    std::string albumtitle;
    std::string pubtime;
    std::string artist;
    std::string pic;
    bool r = false;
    http_ = new spider::HttpResponse(url);
    if(http_==NULL)
    	return 0;
    http_->Get();
    r = http_->GetContent(content);
    if(!r)
    	return 0;
    if(content.length()<10){
        MIG_DEBUG(USER_LEVEL,"content: %s",content.c_str());
        return 0;
    }
    //MIG_DEBUG(USER_LEVEL,"content: %s",content.c_str());
    std::string content_sort;
    content_sort.assign(content.c_str()+1,content.length()-2);
    //MIG_DEBUG(USER_LEVEL,"content_sort: %s",content_sort.c_str());
    r = reader.parse(content_sort.c_str(),root);
    if(!r)
        return r;

    sid = root["sid"].asString();
    ssid = root["ssid"].asString();
    title = root["title"].asString();
    album = root["album"].asString();
    albumtitle = root["albumtitle"].asString();
    pubtime = root["pubtime"].asString();
    artist = root["artist"].asString();
    pic = root["picture"].asString();

    MIG_DEBUG(USER_LEVEL,"sid[%s]-ssid[%s]-title[%s]-album[%s]-albumtitle[%s]-pubtime[%s]-artist[%s]",
             sid.c_str(),ssid.c_str(),title.c_str(),album.c_str(),
             albumtitle.c_str(),pubtime.c_str(),artist.c_str());

    base_storage::MysqlSerial::SpiderDouBan(sid,ssid,base::BasicUtil::GetRawString(title),album,
        base::BasicUtil::GetRawString(albumtitle),pubtime,base::BasicUtil::GetRawString(artist),pic);
    return 1;
}
	
}
