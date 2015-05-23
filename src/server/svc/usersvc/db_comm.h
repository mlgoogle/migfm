/*
 * db_comm.h
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */
#ifndef __USRSVC__DB_COMM__
#define __USRSVC__DB_COMM__
#include "user_basic_info.h"
#include "logic/logic_infos.h"
#include "config/config.h"
#include <list>
namespace usersvc_logic{

class DBComm{
public:
	DBComm();
	virtual ~DBComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool OnThirdLogin(usersvc_logic::UserInfo& userinfo,
			base_logic::LBSInfos* lbsinfo);

	static bool OnQuickRegister(usersvc_logic::UserInfo& userinfo,
				base_logic::LBSInfos* lbsinfo);

	static bool OnLoginRecord(const int64 uid,const int32 plt,const int32 machine,
			base_logic::LBSInfos* lbsinfo);

	static bool OnUpdateUserInfo(usersvc_logic::UserInfo& userinfo);
};

}
#endif


