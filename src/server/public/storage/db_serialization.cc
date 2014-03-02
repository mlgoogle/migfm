#include "storage/db_serialization.h"
#include <string>
#include <sstream>
#include <assert.h>
#include <mysql.h>
#include "basic/basic_info.h"
#include "basic/basic_util.h"
#include "storage/storage.h"
#include "log/mig_log.h"

namespace base_storage{

base_storage::DBStorageEngine*
	MysqlSerial::mysql_db_engine_ = NULL;

bool MysqlSerial::Init(std::list<base::ConnAddr>& addrlist){
	mysql_db_engine_ = DBStorageEngine::Create(IMPL_MYSQL);
    assert(mysql_db_engine_);
    return mysql_db_engine_->Connections(addrlist);
}

bool MysqlSerial::RecordingPacket(const std::string& type){
	bool r = false;
	std::stringstream sql;
	db_row_t* db_rows;
	uint32 num;
	MYSQL_ROW rows;
	sql<<"select count from billing_packet_count where type=\'"<<type.c_str()<<"\'";
	MIG_DEBUG(USER_LEVEL,"sql[%s]\n",sql.str().c_str());
	r = mysql_db_engine_->SQLExec(sql.str().c_str());
	if(!r)
		return false;
	num = mysql_db_engine_->RecordCount();
	if (num>0){
		db_rows = mysql_db_engine_->FetchRows();
		rows = (*(MYSQL_ROW*)db_rows->proc); 
		int count = atol(rows[0]);
		count++;
		sql.str("");
		sql<<"update billing_packet_count set count = "<<count
			<<" where type=\'"<<type.c_str()<<"\'";
		MIG_DEBUG(USER_LEVEL,"sql[%s]\n",sql.str().c_str());
		r = mysql_db_engine_->SQLExec(sql.str().c_str());

	}
	return true;
}

bool MysqlSerial::GetUserInfo(const std::string& username,std::string& userid,int32& sex,std::string& extadd,
	                          std::string& street,std::string& locality,std::string& regin,
	                          int32& pcode,std::string& ctry,std::string& head,
	                          std::string& birthday,std::string& nickname){
    bool r = false;
    std::stringstream sql;
    uint32 num;
    db_row_t* db_rows;
    MYSQL_ROW rows;
    //select usrid,sex,extadd,street,locality,region,pcode,
    //ctry,head,birthday,nickname from migfm_user_infos where usrid =10000
    sql<<"select usrid,sex,extadd, street,locality,region,pcode,ctry,head,birthday,"
        <<"nickname,username from migfm_user_infos where username = '"<<username.c_str()<<"';";

    MIG_DEBUG(USER_LEVEL,"sql[%s]\n",sql.str().c_str());
    r = mysql_db_engine_->SQLExec(sql.str().c_str());
    if(!r)
    	return false;
    num = mysql_db_engine_->RecordCount();
    if(num>0){
    	db_rows = mysql_db_engine_->FetchRows();
    	rows = (*(MYSQL_ROW*)db_rows->proc); 
    	userid = rows[0];
        sex = atol(rows[1]);
    	extadd = rows[2];
    	street = rows[3];
    	locality = rows[4];
    	regin = rows[5];
    	pcode = atol(rows[6]);
    	ctry = rows[7];
    	head = rows[8];
    	birthday = rows[9];
    	nickname = rows[10];
        //username = rows[11];
    	return true;
    }
    return false;
}


 bool MysqlSerial::SetMusicInfo(const std::string& name,
                                const std::string& phone,
                                const std::string& enter,
                                const std::string& music_info){
    bool r = false;
    std::stringstream sql;
    std::string current_time = base::BasicUtil::FormatCurrentTime().c_str();
    MIG_DEBUG(USER_LEVEL,"current time[%s]",current_time.c_str());
    sql<<"insert into migfm_user_record(usrname,phone,content,time,enter) value(\'"<<name.c_str()<<"\',\'"
        <<phone.c_str()<<"\',\'"<<music_info.c_str()<<"\',\'"<<current_time.c_str()<<"\',\'"
        <<enter.c_str()<<"\')";

    r = mysql_db_engine_->SQLExec(sql.str().c_str());
    return r;
}

bool MysqlSerial::SpiderDouBan(const std::string& sidx,const std::string& ssidx,const std::string& title,
                             const std::string& album,const std::string& albumtitle,
                             const std::string& pubtime,const std::string& artist,const std::string& pic){
    bool r = false;
    std::stringstream sql;
    sql<<"insert into migfm_music_douban(sidx,ssidx,title,album,albumtitle,pubtime,artist,pic) value(\'"
        <<sidx.c_str()<<"\',\'"<<ssidx.c_str()<<"\',\'"<<title.c_str()<<"\',\'"<<album.c_str()<<"\',\'"<<
        albumtitle.c_str()<<"\',\'"<<pubtime.c_str()<<"\',\'"<<artist.c_str()<<"\',\'"<<pic.c_str()<<"\')";

    r = mysql_db_engine_->SQLExec(sql.str().c_str());
    return r;
}


bool MysqlSerial::SetMusicTempInfo(const std::string& name,const std::string& enter,
                      const std::string& content){
    bool r = false;
    std::stringstream sql;
    sql<<"inser into migfm_temp_record(name,enter,content) value(\'"
        <<name.c_str()<<"\',\'"<<enter.c_str()<<"\',\'"<<content.c_str()
        <<"\')";
    r = mysql_db_engine_->SQLExec(sql.str().c_str());
    return r;
}


#if defined (MIG_SSO)
bool MysqlSerial::CheckUserPassword(const char*username,const char* password){ 
    std::stringstream os;
    bool r = false;
    uint32 num;
    db_row_t* db_rows;
    MYSQL_ROW rows;
    char* db_idx = NULL;
    char* db_usr_name = NULL;
    char* db_pass_word = NULL;
    os<<"select usrid,username,passwd from migfm_identity where username='"<<username<<"'"; 
    r = mysql_db_engine_->SQLExec(os.str().c_str());
    if(!r){
        MIG_ERROR(USER_LEVEL,"sqlexec error");
        return r;
    }
    num = mysql_db_engine_->RecordCount();
    if(num>0){
        db_rows = mysql_db_engine_->FetchRows();
        rows = (*(MYSQL_ROW*)db_rows->proc);
        db_idx = rows[0];
        db_usr_name = rows[1];
        db_pass_word = rows[2];
        if((strcmp(username,db_usr_name)==0)&&
           (strcmp(db_pass_word,password)==0)){
            return true;
        }
    }

    return  false;
}
#endif


bool MysqlSerial::GetWXMusicInfo(const std::string& id,
								 std::string& title,
								 std::string& desc, 
								 std::string& url){
    std::stringstream os;
	bool r = false;
	uint32 num;
	db_row_t* db_rows;
	MYSQL_ROW rows;
	os<<"select title,description,url from migfm_wx_msg where id = "
		<<id;
	MIG_DEBUG(USER_LEVEL,"%s",os.str().c_str());
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	num = mysql_db_engine_->RecordCount();
	MIG_DEBUG(USER_LEVEL,"###num [%d]####",num);
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){
			title.assign(rows[0]);
			desc.assign(rows[1]);
			url.assign(rows[2]);
		}
		return true;
	}
	return false;
}

bool MysqlSerial::GetMusicAll(std::list<base::MusicInfo >& music_info){
	std::stringstream os;
	bool r = false;
	uint32 num;
	db_row_t* db_rows;
	MYSQL_ROW rows;
	os<<"select sidx,ssidx,albumtitle,title,artist,url,pubtime from migfm_music_douban";
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	num = mysql_db_engine_->RecordCount();
	MIG_DEBUG(USER_LEVEL,"###num [%d]####",num);
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){
			base::MusicInfo music_info;
			music_info.set_sid(rows[0]);
			music_info.set_ssid(rows[1]);
			music_info.set_album_title(rows[2]);
			music_info.set_title(rows[3]);
			music_info.set_artist(rows[4]);
			//music_info.set_url(rows[5]);
			music_info.set_pub_time(rows[6]);
		}
			return true;
	}
	return false;

}

bool MysqlSerial::DelMusicDouBan(std::string& sql){
	std::stringstream os;
	bool r = false;
	uint32 num;
	os<<"delete from migfm_music_douban where id = " 
	   <<sql.c_str();
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}
	return r;
}

bool MysqlSerial::GetMusicDouBan(base::MusicInfo& music_info,std::string& sql){
	std::stringstream os;
	bool r =false;
	uint32 num;
	db_row_t* db_rows;
	MYSQL_ROW rows;
	os<<"select sidx,ssidx,albumtitle,title,artist,url,pubtime,pic from migfm_music_douban where id ="
		<<sql.c_str();
	
	//MIG_DEBUG(USER_LEVEL,"sqlexec[%s]",os.str().c_str());
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = mysql_db_engine_->RecordCount();
	//MIG_DEBUG(USER_LEVEL,"###num [%d]####",num);
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){
			music_info.set_sid(rows[0]);
			music_info.set_ssid(rows[1]);
			music_info.set_album_title(rows[2]);
			music_info.set_title(rows[3]);
			music_info.set_artist(rows[4]);
			//music_info.set_url(rows[5]);
			music_info.set_pub_time(rows[6]);
			music_info.set_pic_url(rows[7]);
			music_info.set_id(sql);
			return true;
		}
	}
	return false;
}

bool MysqlSerial::GetMusicRawDouBan(base::MusicInfo& music_info,std::string& sql){
	std::stringstream os;
	bool r = false;
	uint32 num;
	db_row_t* db_rows;
	MYSQL_ROW rows;
	os<<"select sidx,ssidx,albumtitle,title,artist,url,pubtime from migfm_music_douban where "
		<<sql<<" ORDER BY RAND() LIMIT 1";

	MIG_DEBUG(USER_LEVEL,"sqlexec[%s]",os.str().c_str());
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = mysql_db_engine_->RecordCount();
	MIG_DEBUG(USER_LEVEL,"###num [%d]####",num);
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){
			music_info.set_sid(rows[0]);
			music_info.set_ssid(rows[1]);
			music_info.set_album_title(rows[2]);
			music_info.set_title(rows[3]);
			music_info.set_artist(rows[4]);
			//music_info.set_url(rows[5]);
			music_info.set_pub_time(rows[6]);
			return true;
		}
	}
	return false;
}

bool MysqlSerial::GetMusicRaw(base::MusicInfo& music_info,std::string& sql){
	std::stringstream os;
	bool r = false;
	uint32 num;
	db_row_t* db_rows;
	MYSQL_ROW rows;
	os<<"select sid,ssid,albumtitlle,titile,artist,url,public_time  from migfm_music_infos where "
		<<sql<<"ORDER BY RAND() LIMIT 1";
	MIG_DEBUG(USER_LEVEL,"sqlexec[%s]",os.str().c_str());
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = mysql_db_engine_->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){
			music_info.set_sid(rows[0]);
			music_info.set_ssid(rows[1]);
			music_info.set_album_title(rows[2]);
			music_info.set_title(rows[3]);
			music_info.set_artist(rows[4]);
			music_info.set_url(rows[5]);
			music_info.set_pub_time(rows[6]);
		}
	}
}

bool MysqlSerial::GetChannelInfo(std::vector<base::ChannelInfo>& channel,int& num){
	std::stringstream os;
	bool r = false;
	db_row_t* db_rows;
	MYSQL_ROW rows = NULL;
	os<<"select id,channel_id,channel_name from migfm_channel";
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	if(!r){
		MIG_ERROR(USER_LEVEL,"sqlexec error");
		return r;
	}

	num = mysql_db_engine_->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){

			std::string id = rows[0];
			std::string channel_id = rows[1];
			std::string channel_name = rows[2];
			MIG_INFO(USER_LEVEL,"id[%s] channel[%s] channel_name[%s]",
				      id.c_str(),channel_id.c_str(),channel_name.c_str());
			//base::ChannelInfo ci(id,channel_id,channel_name);
			//channel.push_back(ci);
		}
	}
	 return true;
}

bool MysqlSerial::GetMusicInfos(std::list<base::MusicUsrInfo>& music_list){
   std::stringstream os;
   bool r = false;
   uint32 num;
   db_row_t* db_rows;
   MYSQL_ROW rows;
   os<<"select usrname,phone,content from migfm_user_record limit 0,100;";
   r = mysql_db_engine_->SQLExec(os.str().c_str());
   if(!r){
       MIG_ERROR(USER_LEVEL,"sqlexec error");
       return r;
   }

   num = mysql_db_engine_->RecordCount();
   if(num>0){
       while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){
            std::string name = rows[0];
            std::string phone = rows[1];
            std::string content = rows[2];
            base::MusicUsrInfo mi(name,phone,content);
            music_list.push_back(mi);
       }
   }

   return true;
}

bool MysqlSerial::GetDayRecommend(const int flag,std::string& title,std::string& desc, 
								  std::string& pic,std::string& url){
	std::stringstream os;
	bool r = false;
	uint32 num;
	db_row_t* db_rows;
	MYSQL_ROW rows;
	if (flag)//1，获取歌曲 0，获取文章地址
	os<<"select title,description,picurl,url from migfm_wx_dayrecommend order by id desc limit 0,1;";
	else
		os<<"select title,description,picurl,article from migfm_wx_dayrecommend order by id desc limit 0,1;";
	r = mysql_db_engine_->SQLExec(os.str().c_str());
	if(!r){
	  MIG_ERROR(USER_LEVEL,"sqlexec error");
	  return r;
	}

	num = mysql_db_engine_->RecordCount();
	if(num>0){
	  while(rows = (*(MYSQL_ROW*)(mysql_db_engine_->FetchRows())->proc)){
		  title = rows[0];
		  desc = rows[1];
		  pic = rows[2];
		  url = rows[3];
		  return true;
	  }
	}
	return false;
}

}
