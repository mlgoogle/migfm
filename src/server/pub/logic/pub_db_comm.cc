/*
 * pub_db_comm.cc
 *
 *  Created on: 2014年12月11日
 *      Author: kerry
 */


#include "pub_db_comm.h"
#include "db/base_db_mysql_auto.h"
#include <sstream>
#include <mysql.h>

namespace basic_logic{

void PubDBComm::Init(std::list<base::ConnAddr>& addrlist){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Init(addrlist);
#endif
}

void PubDBComm::Dest(){
#if defined (_DB_POOL_)
	base_db::MysqlDBPool::Dest();
#endif
}

void PubDBComm::GetDimensions(const std::string& type,
		base_logic::Dimensions& dimensions){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return ;
	}
	//call migfm.proc_V2GetDimensions('chl')
	os<<"call proc_V2GetDimensions(\'"<<type<<"\')";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return ;
	}

	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			int64 id = 0;
			std::string name;
			std::string desc;
			if(rows[0]!=NULL)
				id = atoll(rows[0]);
			if(rows[1]!=NULL)
				name = rows[1];
			base_logic::Dimension dimension(id,name,desc);
			dimensions.set_dimension(id,dimension);
		}
	}
}

void PubDBComm::GetAvailableMusicInfo(std::map<int64,base_logic::MusicInfo>& map){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return ;
	}
	//call proc_V2GetAvailableMusicInfos()
	os<<"call proc_V2GetAvailableMusicInfos()";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return ;
	}

	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			base_logic::MusicInfo musicinfo;
			if(rows[0]!=NULL)
				musicinfo.set_id(atoll(rows[0]));
			if(rows[1]!=NULL)
				musicinfo.set_hot(atoll(rows[1]));
			if(rows[2]!=NULL)
				musicinfo.set_clt(atoll(rows[2]));
			if(rows[3]!=NULL)
				musicinfo.set_cmt(atoll(rows[3]));
			if(rows[4]!=NULL)
				musicinfo.set_title(rows[4]);
			if(rows[5]!=NULL)
				musicinfo.set_ablum(rows[5]);
			if(rows[6]!=NULL)
				musicinfo.set_pubtime(rows[6]);
			if(rows[7]!=NULL)
				musicinfo.set_artist(rows[7]);
			if(rows[8]!=NULL)
				musicinfo.set_url(rows[8]);
			if(rows[8]!=NULL)
				musicinfo.set_hq_url(rows[8]);
			if(rows[9]!=NULL)
				musicinfo.set_pic(rows[9]);
			if(rows[10]!=NULL)
				musicinfo.set_lyric(rows[10]);
			map[musicinfo.id()] = musicinfo;
		}
	}
}


void PubDBComm::GetMusicURL(std::map<int64,base_logic::MusicInfo>& map){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return ;
	}
	//call migfm.proc_V2GetMusicURL()
	os<<"call proc_V2GetMusicURL()";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return ;
	}

	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			base_logic::MusicInfo musicinfo;
			if(rows[0]!=NULL)
				musicinfo.set_id(atoll(rows[0]));
			if(rows[1]!=NULL)
				musicinfo.set_url(rows[1]);
			if(rows[2]!=NULL)
				musicinfo.set_hq_url(rows[2]);
			map[musicinfo.id()] = musicinfo;
		}
	}
}

void PubDBComm::GetMusicAbout(std::map<int64,base_logic::MusicInfo>& map){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return ;
	}
	//call proc_V2GetMusicAbout()
	os<<"call proc_V2GetMusicAbout()";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return ;
	}

	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			//检测是否存在 // fix me
			if(rows[0]!=NULL&&rows[1]!=NULL)
				map[atoll(rows[0])].set_hot(atoll(rows[1]));
			if(rows[0]!=NULL&&rows[2]!=NULL)
				map[atoll(rows[0])].set_cmt(atoll(rows[2]));
			if(rows[0]!=NULL&&rows[3]!=NULL)
				map[atoll(rows[0])].set_clt(atoll(rows[3]));
		}
	}
}


void PubDBComm::GetUserInfoByLoginTime(std::map<int64,base_logic::UserAndMusic>& map){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return ;
	}
	//call proc_V2GetUserInfoByLoginTime()
	os<<"call proc_V2GetUserInfoByLoginTime()";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return ;
	}

	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			base_logic::UserAndMusic info;
			if(rows[0]!=NULL)
				info.userinfo_.set_uid(atoll(rows[0]));
			if(rows[1]!=NULL)
				info.userinfo_.set_sex(atol(rows[1]));
			if(rows[2]!=NULL)
				info.userinfo_.set_nickname(rows[2]);
			if(rows[3]!=NULL)
				info.userinfo_.set_source(atol(rows[3]));
			if(rows[4]!=NULL)
				info.userinfo_.set_city(rows[4]);
			if(rows[5]!=NULL)
				info.userinfo_.set_birthday(rows[5]);
			if(rows[6]!=NULL)
				info.userinfo_.set_head(rows[6]);
			if(rows[7]!=NULL)
				info.userinfo_.set_logintime(atoll(rows[7]));

			map[info.userinfo_.uid()] = info;
		}
	}
}


void PubDBComm::GetUserInfoByLocation(std::map<int64,base_logic::UserAndMusic>& map){
	bool r = false;
#if defined (_DB_POOL_)
	base_db::AutoMysqlCommEngine auto_engine;
	base_storage::DBStorageEngine* engine  = auto_engine.GetDBEngine();
#endif
	std::stringstream os;
	MYSQL_ROW rows;

	if (engine==NULL){
		LOG_ERROR("GetConnection Error");
		return ;
	}
	//call proc_V2GetUserInfoByLoginLocation()
	os<<"call proc_V2GetUserInfoByLoginLocation()";
	std::string sql = os.str();
	LOG_MSG2("[%s]", sql.c_str());
	r = engine->SQLExec(sql.c_str());

	if (!r) {
		LOG_ERROR("exec sql error");
		return ;
	}

	int num = engine->RecordCount();
	if(num>0){
		while(rows = (*(MYSQL_ROW*)(engine->FetchRows())->proc)){
			base_logic::UserAndMusic info;
			if(rows[0]!=NULL)
				info.userinfo_.set_uid(atoll(rows[0]));
			if(rows[1]!=NULL)
				info.userinfo_.set_sex(atol(rows[1]));
			if(rows[2]!=NULL)
				info.userinfo_.set_nickname(rows[2]);
			if(rows[3]!=NULL)
				info.userinfo_.set_source(atol(rows[3]));
			if(rows[4]!=NULL)
				info.userinfo_.set_city(rows[4]);
			if(rows[5]!=NULL)
				info.lbsinfo_.set_latitude(atof(rows[5]));
			if(rows[6]!=NULL)
				info.lbsinfo_.set_longitude(atof(rows[6]));
			if(rows[7]!=NULL)
				info.userinfo_.set_birthday(rows[7]);
			if(rows[8]!=NULL)
				info.userinfo_.set_head(rows[8]);
			if(rows[9]!=NULL)
				info.userinfo_.set_logintime(atoll(rows[9]));
			map[info.userinfo_.uid()] = info;
		}
	}
}

}
