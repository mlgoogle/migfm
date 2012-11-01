#include "mysql_opertion.h"
#include <sstream>
#include "basictypes.h"
#include "mysql_operator_info.h"

MysqlOpertion::MysqlOpertion(){
	Init();
}

MysqlOpertion::~MysqlOpertion(){

}

void MysqlOpertion::Init(){
	engine_.reset(StorageEngine::Create(IMPL_MYSQL));
}

bool MysqlOpertion::Connections(std::list<ConnAddr>& addrlist){
	
	bool r = engine_.get()->Connections(addrlist);
	return r;
}

bool MysqlOpertion::AddUserInfo(std::string& usrname,std::string& password,int sex,
					 			int address,std::string& name,std::string& id_cards,
					 			std::string& head){
	scoped_ptr<UserInfo> usr_ptr;
	int idx = GetRandomTime();
	int nowidx = idx>0?idx:(0-idx);
	std::stringstream sid;
	sid<<nowidx;
	UserInfo usr;
	usr.opcode_ = ADD_USER_INFO;
	usr.id_ = sid.str();
	usr.usrname_ = usrname;
	usr.password_ = password;
	usr.sex_ = sex;
	usr.address_ = address;
	usr.name_ = name;
	usr.id_cards_ = id_cards;
	usr.head_ = head;
    engine_.get()->AddValue(sid.str().c_str(),sid.str().length(),NULL,0,&usr);
    return true;
}
