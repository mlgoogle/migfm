/*
 * db_comm.h
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#ifndef MUSIC_DB_HEAD_H_
#define MUSIC_DB_HEAD_H_
#include "logic/logic_infos.h"
#include "config/config.h"
#include <list>
namespace musicsvc_logic{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetAllDimension(std::list<base_logic::Dimension>& list);
	static bool GetMyFriend(const int64 uid,const int64 from,const int64 count,
			std::map<int64,base_logic::UserAndMusic>& map);

	static bool RecordMusicHistory(const int64 uid,const int64 songid);

	static bool GetPushMessage(const int64 uid,const int32 from,const int32 count,
			std::map<int64,base_logic::PersonalMessage>& map);
};

}
#endif


