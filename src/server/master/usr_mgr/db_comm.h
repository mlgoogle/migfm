#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#include "thread_handler.h"
#include "thread_lock.h"
#include "logic_comm.h"
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <list>
namespace storage{

class DBComm{
public:
	DBComm(){}
	virtual ~DBComm(){}
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist,
			  const int32 db_conn_num= 10);
	
	static void Dest();
	
	static bool GetUserIndent(const std::string& username,int64& uid);

	static bool RegeditUser(const std::string& username,const std::string& password,
		                    const std::string& nickname,const std::string& source);

	static bool RegistUser(const char* plat_id,const char* plat_session,
		             const char* password,int& sex,std::string& username,
					 std::string& nickname, int64& userid, int64& type, 
					 std::string& location,std::string& birthday,std::string& head,
					 int32& return_code);

	static bool AddUserInfos(const int uid,const std::string& username,
		                        const std::string& nickname,const std::string& gender,
								const std::string& type,const std::string& birthday,
								const std::string& location, const std::string& source,
								const std::string& head);

	static bool UpDateUserInfos(const int uid,const std::string& username,
		                        const std::string& nickname,const std::string& gender,
		                        const std::string& type,const std::string& birthday,
		                        const std::string& location, const std::string& source,
		                        const std::string& head);

	static bool UpdateUserInfos(const int64 uid,const std::string& nickname,
			                    const std::string& gender,
			                    const std::string& birthday);

	static bool GetUserInfos(const std::string& username,std::string& uid,
		                        std::string& nickname,std::string& gender,
		                        std::string& type,std::string& birthday,
		                        std::string& location, std::string& source,
		                        std::string& head);

	static bool CheckUserInfo(const std::string& clientid,const std::string& token,
		                      const std::string& username,const std::string& password,
		                      std::string& uid,std::string& nickname,
							  std::string& gender,std::string& type,
							  std::string& birthday,std::string& location, 
							  std::string& source,std::string& head,int& return_code);

	static bool RecordDefaultMessage(const int64 uid,const int64 msg_id);

public:
#if defined (_DB_POOL_)
	static base_storage::DBStorageEngine* DBConnectionPop(void);
	static void DBConnectionPush(base_storage::DBStorageEngine* engine);
#endif
public:
	static std::list<base::ConnAddr>  addrlist_;
private:
#if defined (_DB_POOL_)
	static std::list<base_storage::DBStorageEngine*>  db_conn_pool_;
	static threadrw_t*                                db_pool_lock_;
#endif
};

class AutoDBCommEngine{
public:
	AutoDBCommEngine();
	virtual ~AutoDBCommEngine();
	base_storage::DBStorageEngine*  GetDBEngine(){
		if(engine_){
			engine_->Release();//释放多余记录集
			if(!engine_->CheckConnect()){//失去连接重新连接
				//重新创建连接
				LOG_DEBUG("lost connection");
				if(!engine_->Connections(DBComm::addrlist_))
					return NULL;
			}
			return engine_;
		}
		return engine_;
	}
private:
	base_storage::DBStorageEngine*  engine_;
};

}
#endif
