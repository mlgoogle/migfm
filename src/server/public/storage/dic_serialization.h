#ifndef _MIG_FM_PUBLIC_STORAGE_DIC_SERIALIZATION__H__
#define _MIG_FM_PUBLIC_STORAGE_DIC_SERIALIZATION__H__

#include <stdio.h>
#include "storage/storage.h"

namespace base_storage{

class MemDicSerial{
public:
    MemDicSerial();
    virtual ~MemDicSerial();
    
    static bool Init(std::list<base::ConnAddr>& addrlist);
    
    static bool SetString(const char* key,const size_t key_len,
                          const char* data, size_t len);
    
    static bool GetString(const char* key,const size_t key_len,
                          char** data,size_t* len);

#if defined (MIG_SSO)
    static bool IdpCheckSerial(const char* ticket,const char* idp_identity,
                               const char* idp_session,const char* provider);
    
    static bool IdpCheckUnserial(const char* ticket,std::string& idp_identity,
                                 std::string& idp_session,std::string& provider);
    static bool DeleteIdpCheck(const char* ticket);
#endif
private:
    static base_storage::DictionaryStorageEngine*     mem_engine_;
};


class RedisDicSerial{
public:
	RedisDicSerial();
	virtual ~RedisDicSerial();
	static bool Init(std::list<base::ConnAddr>& addrlist);

	static bool SetMusicInfos(const std::string& key,std::string& music_infos);
	static bool GetMusicInfos(const std::string& key,std::string& music_infos);
	static bool DelMusciInfos(const std::string& key);

	static bool GetMoodAndScensId(const std::string& key,std::string& word_id);

	static bool GetArtistMoodAndScensNum(const std::string& key,std::string& num);

	static bool SetMusicMapInfo(const std::string& art_name,
									const std::string& key,
									const std::string song_id);

	static bool GetMusicMapInfo(const std::string& art_name,
		                         const std::string& key,
		                         std::string& song_id);

	static bool DelMusicMapInfo(const std::string& art_name,
								const std::string& key);

	static bool SetMusicMapRadom(const std::string& art_name,const std::string& song_id);

	static bool GetMusicMapRadom(const std::string& art_name,std::string& song_id);

	static bool DelMusicMapRadom(const std::string& art_name);
	
private:
	static base_storage::DictionaryStorageEngine*    redis_engine_;
	
};

}
#endif
