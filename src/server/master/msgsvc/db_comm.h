/*
 * db_comm.h
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#ifndef MSG_DB_HEAD_H_
#define MSG_DB_HEAD_H_
#include "msg_basic_info.h"
#include "logic/logic_infos.h"
#include "config/config.h"
#include <list>
namespace msgsvc_logic{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

public:
	//批量写入存储
	static bool RecordPushMessage(PUSHMESSAGELIST& list);

	//写入好友列表
	static bool AddUserFriend(const int64 uid,const int64 tid,std::string& nickname);

};

}
#endif


