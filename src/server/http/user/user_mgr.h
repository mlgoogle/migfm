#ifndef MIG_FM_USER_MGR_H__
#define MIG_FM_USER_MGR_H__
#include "mysql_opertion.h"
#include "user_engine.h"
#include "scoped_ptr.h"

namespace userinfo{
	
class UserMgr{
public:
	UserMgr();
	virtual ~UserMgr();
	static UserMgr* GetInstance();
    static void FreeInstance();
public: 
	bool Init();
	
	bool GetUserInfo(const char* str,const int32 len,std::string& out);
	
	bool PostUserInfo(const char* str,int32 len);
		
	bool PutUserInfo(const char* str,int32 len);
		
	bool DeleteUserInfo(const char* str,const int32 len);

private:
	scoped_ptr<base::MysqlOpertion>   mysql_opertion_;
	static UserMgr* 			      instance_;
	scoped_ptr<UserEngine>            user_engine_;
};

}
#endif