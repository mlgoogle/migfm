#include "usr_logic.h"
#include "mysql_opertion.h"

namespace{

UsrLogic::UsrLogic(){
	mysql_opertion_.reset(base::MysqlOpertion());
}

UsrLogic::~UsrLogic(){

}


bool UsrLogic::GetUserInfo(std::string& id){

}

}
