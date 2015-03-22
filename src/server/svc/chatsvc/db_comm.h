/*
 * db_comm.h
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#ifndef CHAT_DB_HEAD_H_
#define CHAT_DB_HEAD_H_
#include "chat_basic_infos.h"
#include "logic/logic_infos.h"
#include "config/config.h"
#include <list>
namespace chatsvc_logic{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

public:
	static bool OnGetGroupMessage(const int64 platform,const int64 groupid,const int32 from,const int32 count,
			const int64 msgid,std::list<chatsvc_logic::GroupMessageInfos>& list);

	static bool OnGetLeaveMessage(const int64 platform,const int64 fid,const int64 tid,const int64 from,
			const int64 count,const int64 msgid,std::list<chatsvc_logic::AloneMessageInfos>& list);
};

}
#endif


