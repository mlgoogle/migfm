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
};

}
#endif


