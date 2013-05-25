#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "log/mig_log.h"
#include "config/config.h"
#include "storage/dic_serialization.h"
#include "storage/db_serialization.h"


int main(int agrc,char* argv[]){

	bool r = false;
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if (config==NULL)
		return 1;
	std::string path="config.xml";
	r = config->LoadConfig(path);
	if(!r)
		return 1;
	base::MusicInfo mi;
	r = base_storage::MysqlSerial::Init(config->mysql_db_list_);
	r = base_storage::RedisDicSerial::Init(config->redis_list_);

	for (int i = 147;i<699194;i++){
		std::stringstream os;
		std::string sql;
		os<<i;
		sql = os.str();
 		r = base_storage::MysqlSerial::GetMusicDouBan(mi,sql);
 		if (!r){
 			MIG_ERROR(USER_LEVEL,"GetMusicDouBan Error");
 			continue;
 		}

		//r = base_storage::MysqlSerial::DelMusicDouBan(sql);

#if defined (_ARTLIST_)
 		std::string sjson;
 		mi.SerializedJson(sjson);
 		r = base_storage::RedisDicSerial::SetMusicInfos(mi.id(),sjson);
/* 		MIG_INFO(USER_LEVEL,"r [%d] sid[%s] artlist[%s]",r,mi.sid().c_str(),mi.artist().c_str());*/
 //		std::string mjson;
 //		r = base_storage::RedisDicSerial::GetMusicInfos(mi.sid(),mjson);
// 		MIG_DEBUG(USER_LEVEL,"r [%d] mjson[%s]",r,mjson.c_str());
// 
// 		r = base_storage::RedisDicSerial::DelMusciInfos(mi.sid());
// 		MIG_DEBUG(USER_LEVEL,"r [%d] sid[%s]",r,mi.sid().c_str());
#endif

#if defined (_ARTLIST_)
		//song
		std::string song_id;
		song_id = mi.id();
		std::stringstream mapname;

		//MIG_DEBUG(USER_LEVEL,"album[%s] artist[%s] title[%s]",
			//		mi.album_title().c_str(),mi.artist().c_str(),mi.title().c_str());


		
		//歌手对应歌曲
		//指定存储
		mapname<<"ad_"<<mi.artist().c_str();
		//MIG_DEBUG(USER_LEVEL,"mapname[%s]",mapname.str().c_str());
		std::string keymap = mapname.str();
		base_storage::RedisDicSerial::SetMusicMapInfo(keymap,mi.title(),mi.id());
		//随机存储
		std::stringstream rmap_name;
		rmap_name<<"ar_"<<mi.artist().c_str();
		std::string rkeymap = rmap_name.str();
		base_storage::RedisDicSerial::SetMusicMapRadom(rkeymap,mi.id());
		


		//歌曲对应歌手
		//指定存储
		std::stringstream song_map_name;
		song_map_name<<"sd_"<<mi.title().c_str();
		//MIG_DEBUG(USER_LEVEL,"song_map_name [%s]",song_map_name.str().c_str());
		std::string skeymap = song_map_name.str();
		base_storage::RedisDicSerial::SetMusicMapInfo(skeymap,mi.artist(),mi.id());
		//随机存储
		std::stringstream rsong_map_name;
		rsong_map_name<<"sr_"<<mi.title().c_str();
		std::string rskeymap = rsong_map_name.str();
		base_storage::RedisDicSerial::SetMusicMapRadom(rskeymap,mi.id());

		//存储专辑信息
		std::stringstream albumtime_map;
		albumtime_map<<"al_"<<mi.album_title().c_str();
		std::string alkeymap = albumtime_map.str();
		base_storage::RedisDicSerial::SetMusicMapRadom(alkeymap,mi.id());

// 		//读取信息
// 		//1 根据指定歌手--歌曲名获取
// 		std::string getsongid;
// 		r = base_storage::RedisDicSerial::GetMusicMapInfo(mapname.str(),
// 				mi.title(),getsongid);
// 		if (r){
// 			std::string muiscinfo;
// 			base_storage::RedisDicSerial::GetMusicInfos(getsongid,muiscinfo);
// 			MIG_DEBUG(USER_LEVEL,"ad_songid[%s]",muiscinfo.c_str());
// 		}
// 
// 		MIG_DEBUG(USER_LEVEL,"===============================\n\n");
// 
// 		//2 根据指定歌曲名-找歌手
// 		std::string getsongid1;
// 		r = base_storage::RedisDicSerial::GetMusicMapInfo(song_map_name.str(),
// 								mi.artist(),getsongid1);
// 		if (r){
// 			std::string muiscinfo;
// 			base_storage::RedisDicSerial::GetMusicInfos(getsongid1,muiscinfo);
// 			MIG_DEBUG(USER_LEVEL,"sd_songid[%s]",muiscinfo.c_str());
// 		}
// 
// 		MIG_DEBUG(USER_LEVEL,"===============================\n\n");
// 
// 		//歌手随机一首歌
// 		std::string getsongid2;
// 		r = base_storage::RedisDicSerial::GetMusicMapRadom(rmap_name.str(),getsongid2);
// 		if (r){
// 			std::string muiscinfo;
// 			base_storage::RedisDicSerial::GetMusicInfos(getsongid2,muiscinfo);
// 			MIG_DEBUG(USER_LEVEL,"ar_songid[%s]",muiscinfo.c_str());
// 		}
// 
// 		MIG_DEBUG(USER_LEVEL,"===============================\n\n");
// 
// 		//根据歌曲随机一首歌
// 
// 		std::string getsongid3;
// 		r = base_storage::RedisDicSerial::GetMusicMapRadom(rsong_map_name.str(),
// 											getsongid3);
// 		if (r){
// 			std::string muiscinfo;
// 			base_storage::RedisDicSerial::GetMusicInfos(getsongid2,muiscinfo);
// 			MIG_DEBUG(USER_LEVEL,"sr_songid[%s]",muiscinfo.c_str());
// 		}
// 
// 		MIG_DEBUG(USER_LEVEL,"===============================\n\n");
#endif
	}
	MIG_DEBUG(USER_LEVEL,"===========END===============");
	return true;
}