/*
 * music_dic_comm.h
 *
 *  Created on: 2015年3月20日
 *      Author: pro
 */

#ifndef MUSIC_DIC_COMM_H_
#define MUSIC_DIC_COMM_H_
#include "config/config.h"
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
};

}




#endif /* MUSIC_DIC_COMM_H_ */
