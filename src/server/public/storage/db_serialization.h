#ifndef _MIG_FM_PLUBLIC_STORAGE_DB_SERIALIZATION_H__
#define _MIG_FM_PLUBLIC_STORAGE_DB_SERIALIZATION_H__
#include <stdio.h>
#include <vector>
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
namespace base_storage{

class MysqlSerial{
public:
    MysqlSerial();
	
    virtual ~MysqlSerial();
	
    static bool Init(std::list<base::ConnAddr>& addrlist);
    
	static bool RecordingPacket(const std::string& type);

    static bool GetUserInfo(const std::string& username,std::string& userid,int32& sex,std::string& extadd,
	                        std::string& street,std::string& locality,std::string& regin,
	                        int32& pcode,std::string& ctry,std::string& head,
	                        std::string& birthday,std::string& nickname);

  
    static bool SpiderDouBan(const std::string& sidx,const std::string& ssidx,const std::string& title,
                             const std::string& album,const std::string& albumtitle,
                             const std::string& pubtime,const std::string& artist,
                             const std::string& pic);

    static bool SetMusicInfo(const std::string& name,const std::string& phone,
                             const std::string& enter,const std::string& music_info);

    static bool GetMusicInfos(std::list<base::MusicUsrInfo>& music_list);

    static bool SetMusicTempInfo(const std::string& name,const std::string& enter,
                                 const std::string& content);

	static bool GetChannelInfo(std::vector<base::ChannelInfo>& channel,int& num);

	static bool GetMusicRaw(base::MusicInfo& music_info,std::string& sql);

	static bool GetMusicRawDouBan(base::MusicInfo& music_info,std::string& sql);

	static bool GetMusicDouBan(base::MusicInfo& music_info,std::string& sql);

	static bool DelMusicDouBan(std::string& sql);

	static bool GetMusicAll(std::list<base::MusicInfo >& music_info);

	static bool GetWXMusicInfo(const std::string& id,std::string& title,
		                       std::string& desc,std::string& url);

	static bool GetDayRecommend(const int flag,std::string& title,std::string& desc,
		                        std::string& pic,std::string& url);

#if defined (MIG_SSO)
    static bool CheckUserPassword(const char* username,const char* password);
#endif 
private:
    static base_storage::DBStorageEngine*   mysql_db_engine_;
};	


}

#endif
