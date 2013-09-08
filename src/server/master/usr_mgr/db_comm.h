#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
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
	static void Init(std::list<base::ConnAddr>& addrlist);
	
	static void Dest();
	
	static bool GetUserIndent(const std::string& username,int64& uid);

	static bool RegeditUser(const std::string& username,const std::string& password,
		                    const std::string& nickname,const std::string& source);

	static bool RegistUser(const char* plat_id,const char* plat_session,
		             const char* password,int& sex,std::string& username,
					 std::string& nickname, int64& userid, int64& type, 
					 std::string& location,std::string& birthday,std::string& head);

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

	static bool GetUserInfos(const std::string& username,std::string& uid,
		                        std::string& nickname,std::string& gender,
		                        std::string& type,std::string& birthday,
		                        std::string& location, std::string& source,
		                        std::string& head);
private:
	static std::list<base::ConnAddr>  addrlist_;
};

}
#endif