/*
 * db_comm.h
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#ifndef SOC_DB_HEAD_H_
#define SOC_DB_HEAD_H_
#include "soc_basic_infos.h"
#include "logic/logic_infos.h"
#include "config/config.h"
#include <list>
namespace socsvc_logic{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

public:
	static bool GetTypeBarrage(const int64 platform,const int64 groupid,
			const int64 count,std::list<socsvc_logic::BarrageInfos>& list);

	static double GetDistance(const int64 uid,const int64 tid);

	static bool GetMyFriend(const int64 uid,const int64 from,const int64 count,
			std::list<base_logic::UserAndMusic>& list);
};

}
#endif


