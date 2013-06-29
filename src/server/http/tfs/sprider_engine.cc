#include "sprider_engine.h"
#include "log/mig_log.h"
#include "json/json.h"
#include "basic/basic_util.h"
#include <sstream>

namespace spider{
	
SpiderEngine::SpiderEngine(){
	url_ = "http://douban.fm/j/mine/playlist";
	//url_ = "http://mr4.douban.com/201304222012/fb81ac0bd7561008ef08eddd3cada9bb/view/song/small/p1905043.mp3";
/*	url_ = "http://mr3.douban.com/201304222119/37b74da6f262e38e65f6c59792365f47/view/song/small/p1904739.mp3";*/
	mysql_db_engine_ = base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
	engine_ = new spider::SpiderHttpEngineImpl(mysql_db_engine_);
	fs_engine_ = filestorage::MFSEngine::Create(filestorage::TFS_TYPE);
}

SpiderEngine::~SpiderEngine(){

}

bool SpiderEngine::Init(std::list<base::ConnAddr>& addrlist){
    nameserver = "42.121.126.248:8108";
	mysql_db_engine_->Connections(addrlist);
	//fs_engine_->Initialize(nameserver.c_str());
	return true;
}

void SpiderEngine::GetSprider(){
	std::string content;
	Json::Reader reader;
	Json::Value  root;
	bool r = false;
	int music_infos_size = 0;
	std::stringstream sql;
	int32 flags = tfs::common::T_DEFAULT;
	char* suffix = "mp3";
	r = engine_->RequestHttp(url_,content);
	if (!r)
		return ;
	//parer json
	r = reader.parse(content.c_str(),root);
	if (!r){
		MIG_ERROR(USER_LEVEL,"parse json error[%s]",content.c_str());
		return ;
	}
	music_infos_size = root["song"].size();
	if (music_infos_size<=0){
		MIG_ERROR(USER_LEVEL,"song valiled size[%d]",music_infos_size);
		return ;
	}
	Json::Value song = root["song"];
	for (int i = 0;i<music_infos_size;i++){
		std::string tfs_name;
		char* name = new char[64];
		int32 name_length = 64;
		int ret = 0;
		std::string album =  song[i]["album"].asString();
		std::string picture = song[i]["picture"].asString();
		std::string ssid = song[i]["ssid"].asString();
		std::string artist = song[i]["artist"].asString();
		std::string url = song[i]["url"].asString();
		std::string company = song[i]["company"].asString();
		std::string title = song[i]["title"].asString();
		std::string rating_avg = song[i]["ratting_avg"].asString();
		int length = song[i]["length"].asInt();
		std::string public_time = song[i]["public_time"].asString();
		std::string sid = song[i]["sid"].asString();
		std::string aid = song[i]["aid"].asString();
		std::string kbps = song[i]["kbps"].asString();
		std::string albumtitle = song[i]["albumtitle"].asString();
		/*MIG_DEBUG(USER_LEVEL,"album[%s] picture[%s] ssid[%s] url[%s] company[%s] title[%s] length[%d] sid[%s]       albumtitle[%s]",
			album.c_str(),picture.c_str(),ssid.c_str(),url.c_str(),
			company.c_str(),title.c_str(),length,sid.c_str(),albumtitle.c_str());*/

		//download
		/*std::stringstream filename;
		filename<<"/root/p"<<sid<<".mp3";
		int file_size = engine_->DownLoadFile(url,filename.str());
 		do{
 			ret = fs_engine_->SaveFile(name,name_length,filename.str().c_str(),
 				flags,suffix,nameserver.c_str());
 			MIG_DEBUG(USER_LEVEL,"###########name[%s] ret[%d] file_size[%d]###########",
 						name,ret,file_size);
						}while (ret!=file_size);*/

		sprintf(name,"tfs");
		sql<<"insert into migfm_music_infos(tfs_id,album,picture,ssid,artist,url,company,titile,rating_avg,length,public_time,sid,aid,kbps,albumtitlle) value(\'"<<name<<"\',\'"
			<<album.c_str()<<"\',\'"<<picture.c_str()<<"\',\'"<<ssid.c_str()
			<<"\',\'"<<base::BasicUtil::GetRawString(artist.c_str())
			<<"\',\'"<<url.c_str()<<"\',\'"<<base::BasicUtil::GetRawString(company.c_str())
			<<"\',\'"<<base::BasicUtil::GetRawString(title.c_str())
			<<"\',\'"<<rating_avg.c_str()<<"\',\'"<<length<<"\',\'"
			<<public_time.c_str()<<"\',\'"<<sid.c_str()<<"\',\'"
			<<aid.c_str()<<"\',\'"<<kbps.c_str()<<"\',\'"
			<<base::BasicUtil::GetRawString(albumtitle.c_str())<<"\')";

		mysql_db_engine_->SQLExec(sql.str().c_str());
// 		bool success = (unlink(filename.str().c_str())==0);
 		sql.str("");
// 		filename.str("");
 		if (name){
 			delete [] name;
 			name = NULL;
 		}
	}

	return;
}

}