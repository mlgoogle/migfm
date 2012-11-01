#include "user_mgr.h"

namespace userinfo{
	
UserMgr::UserMgr(){
    Init();
}

UserMgr::~UserMgr(){

}

UserMgr* UserMgr::instance_ = NULL;
	
UserMgr* UserMgr::GetInstance(){
	if(instance_==NULL)
		instance_ = new UserMgr();
	return instance_;
}

void UserMgr::FreeInstance(){
	delete instance_;	
}

bool UserMgr::Init(){
	mysql_opertion_.reset(new base::MysqlOpertion());
	user_engine_.reset(new UserEngine());
	return true;
}

bool UserMgr::GetUserInfo(const char* str,const int32 len,std::string& out){
	user_engine_.get()->HandlerInput(str,len);
	return true;	
}

bool UserMgr::PutUserInfo(const char* str,int32 len){
	user_engine_.get()->HandlerInput(str,len);
	return true;
}

bool UserMgr::DeleteUserInfo(const char* str,const int32 len){
	user_engine_.get()->HandlerInput(str,len);
	return true;
}

bool UserMgr::PostUserInfo(const char* str,int32 len){
	user_engine_.get()->HandlerInput(str,len);
	return true;
}

}