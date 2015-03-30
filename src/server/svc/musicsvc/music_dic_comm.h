/*
 * music_dic_comm.h
 *
 *  Created on: 2015年3月20日
 *      Author: pro
 */

#ifndef MUSIC_DIC_COMM_H_
#define MUSIC_DIC_COMM_H_
#include "logic/logic_unit.h"
#include "config/config.h"
#include "storage/storage.h"
#include <libmemcached/memcached.h>
#include <libmemcachedutil-1.0/pool.h>
#include <list>
#include <map>
namespace musicsvc_logic{

class MusicDicComm{
public:
	MusicDicComm();
	~MusicDicComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static void SetCollect(const int64 uid,const int64 songid,const std::string& json);

	static void DelCollect(const int64 uid,const int64 songid);
};

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

	static void BatchGetCurrentSong(std::map<int64,base_logic::UserAndMusic>& map);
private:
	static base_storage::DictionaryStorageEngine* engine_;
};

}




#endif /* MUSIC_DIC_COMM_H_ */
