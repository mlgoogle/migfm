#ifndef _MIG_FM_PLUBLIC_STORAGE_DB_SERIALIZATION_H__
#define _MIG_FM_PLUBLIC_STORAGE_DB_SERIALIZATION_H__
#include <stdio.h>
#include "storage/storage.h"
#include "basic/basictypes.h"

namespace base_storage{

class MysqlSerial{
public:
    MysqlSerial();
	
    virtual ~MysqlSerial();
	
    static bool Init(std::list<base::ConnAddr>& addrlist);
    
    static bool GetUserInfo(const int32 usr_id,std::string& username,int32& sex,std::string& extadd,
	                        std::string& street,std::string& locality,std::string& regin,
	                        int32& pcode,std::string& ctry,std::string& head,
	                        std::string& birthday,std::string& nickname);

    static bool SetMusicInfo(const std::string& name,const std::string& phone,
                             const std::string& enter,const std::string& music_info);

#if defined (MIG_SSO)
    static bool CheckUserPassword(const char* username,const char* password);
#endif 
private:
    static base_storage::DBStorageEngine*   mysql_db_engine_;
};	


}

#endif
