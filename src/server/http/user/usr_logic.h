#ifndef MIG_FM_USR_LOGIC_H__
#define MIG_FM_USR_LOGIC_H__
#include <stdio.h>
#include <stdlib.h>
#include "scoped_ptr.h"

class base::MysqlOpertion;

namespace userinfo{

class UsrLogic{
public:
	UsrLogic();
	virtual ~UsrLogic();
private:
	bool GetUserInfo(std::string& id);
	bool AddUserInfo();
	bool DelUserInfo();
	bool EditUserInfo();

private:
	scoped_ptr<base::MysqlOpertion>  mysql_opertion_;
	
};
}
#endif
