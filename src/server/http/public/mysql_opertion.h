#ifndef MIG_FM_MYSQL_OPERTION_H__
#define MIG_FM_MYSQL_OPERTION_H__
#include <stdio.h>
#include <stdlib.h>
#include "storage.h"
#include "scoped_ptr.h"

namespace base{
class MysqlOpertion{
public:
	MysqlOpertion();
	~MysqlOpertion();
	void Init();
	
	bool Connections(std::list<ConnAddr>& addrlist);
						
	bool AddUserInfo(std::string& usrname,std::string& password,int sex,
					 int address,std::string& name,std::string& id_cards,
					 std::string& head);
private:
	scoped_ptr<StorageEngine>   engine_;	
};
}
#endif
