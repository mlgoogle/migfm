#include "storage/dic_serialization.h"
#include "basic/basic_info.h"
#include "storage/storage.h"
#include "storage/mem_storage_impl.h"
#include "storage/redis_storage_impl.h"
#include <assert.h>
#include <sstream>

namespace base_storage{

base_storage::DictionaryStorageEngine*  
    MemDicSerial::mem_engine_ = NULL;
	
base_storage::DictionaryStorageEngine*  
	RedisDicSerial::redis_engine_ = NULL;
		
		
///memcached
bool MemDicSerial::Init(std::list<base::ConnAddr>& addrlist){
    mem_engine_ = DictionaryStorageEngine::Create(IMPL_MEM);
    assert(mem_engine_!=NULL);	
    return mem_engine_->Connections(addrlist);
}

bool MemDicSerial::SetString(const char* key,const size_t key_len,
                             const char* data,size_t len){
   return mem_engine_->SetValue(key,key_len,data,len);
}

bool MemDicSerial::GetString(const char* key,const size_t key_len,
                             char** data,size_t* len){
   return mem_engine_->GetValue(key,key_len,data,len);
}

#if defined (MIG_SSO)
bool MemDicSerial::IdpCheckSerial(const char* ticket,const char* idp_identity,
                                  const char* idp_session,const char* provider){
    std::stringstream os;
    os<<idp_identity<<"|"<<idp_session<<"|"
      <<provider<<"|";
    /*MIG_DEBUG(USER_LEVEL,"idp_identity[%s]",idp_identity);
    MIG_DEBUG(USER_LEVEL,"===============================\n");
    MIG_DEBUG(USER_LEVEL,"idp_session[%s]",idp_session);
    MIG_DEBUG(USER_LEVEL,"===============================\n"); 
    MIG_DEBUG(USER_LEVEL,"ticket[%s] length[%d]",ticket,os.str().length());
    MIG_DEBUG(USER_LEVEL,"os[%s]",os.str().c_str()); 
    MIG_DEBUG(USER_LEVEL,"======================================\n");
    std::string txt = "idp_30078888888888";
    mem_engine_->SetValue(txt.c_str(),txt.length(),os.str().c_str(),os.str().length());*/
    //MIG_DEBUG(USER_LEVEL,"key[%s] length[%d]",ticket,os.str().length());
    return mem_engine_->SetValue(ticket,strlen(ticket)+1,
                          os.str().c_str(),os.str().length());

    /*char* mem_value = NULL;
    size_t mem_value_length = 0;
    mem_engine_->GetValue(ticket,strlen(ticket)+1,&mem_value,&mem_value_length);
    MIG_DEBUG(USER_LEVEL,"length[%d]\n",mem_value_length);
    return true;*/
  
}

bool MemDicSerial::IdpCheckUnserial(const char* ticket,std::string& idp_identity,
                             std::string& idp_session,std::string& provider){
     char* check_value;
     char* head = NULL;
     char* temp_head = NULL;
     int32 temp_len = 0;  
     size_t len;
     int32 i = 0;
     bool r = mem_engine_->GetValue(ticket,strlen(ticket)+1,&check_value,&len);
     if(r){
         temp_head = head = check_value;
         while(i!=3){
            temp_head = strchr(head,'|');
            temp_len = temp_head - head;
            if(i==0){
                idp_identity.assign(head,temp_len);
            }else if(i==1){
                idp_session.assign(head,temp_len);
            }else if(i=2){
                provider.assign(head,temp_len);

            }
            i++;
            temp_head++;
            head = temp_head;
         }
         return true;
     }
     return false;
}

bool MemDicSerial::DeleteIdpCheck(const char* ticket){
	bool r = mem_engine_->DelValue(ticket,strlen(ticket)+1);
    return true;
}
#endif

//redis
bool RedisDicSerial::Init(std::list<base::ConnAddr>& addrlist){
	redis_engine_ = DictionaryStorageEngine::Create(IMPL_RADIES);
	assert(redis_engine_ != NULL);
	return redis_engine_->Connections(addrlist);
}

bool RedisDicSerial::SetMusicInfos(const std::string&key,std::string& music_infos){
	bool r = redis_engine_->SetValue(key.c_str(),key.length(),
					music_infos.c_str(),music_infos.length());
	
	MIG_DEBUG(USER_LEVEL,"r[%d] key[%s] value[%s]\n",r,key.c_str(),
				music_infos.c_str());
	return r;
}

bool RedisDicSerial::DelMusciInfos(const std::string& key){
	return  redis_engine_->DelValue(key.c_str(),key.length());
}

bool RedisDicSerial::GetMusicInfos(const std::string& key,std::string& music_infos){
    char* value;
	size_t value_len = 0;
	bool r = redis_engine_->GetValue(key.c_str(),key.length(),
										&value,&value_len);
	if (r){
		music_infos.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}
	
	}else{
		MIG_DEBUG(USER_LEVEL,"GetValue error[%s]",key.c_str());
	}
	
	return r;
}

bool RedisDicSerial::SetMusicMapInfo(const std::string& art_name, 
									 const std::string& key, 
									 const std::string song_id){
     return redis_engine_->AddHashElement(art_name.c_str(),key.c_str(),
											key.length(),song_id.c_str(),
											song_id.length());
}

bool RedisDicSerial::GetMusicMapInfo(const std::string& art_name, 
									 const std::string& key, 
									 std::string& song_id){
	 char* value;
	 size_t value_len = 0;
	 bool r = false;
	 r = redis_engine_->GetHashElement(art_name.c_str(),key.c_str(),key.length(),
												&value,&value_len);
	 if (r){
		 song_id.assign(value,value_len-1);
		 if (value){
			 free(value);
			 value = NULL;
		 }

	 }else{
		 MIG_DEBUG(USER_LEVEL,"GetValue error[%s]",key.c_str());
	 }
	 return r;
}

bool RedisDicSerial::DelMusicMapInfo(const std::string& art_name, const std::string& key){
	return redis_engine_->DelHashElement(art_name.c_str(),key.c_str(),key.length());
}

bool RedisDicSerial::SetMusicMapRadom(const std::string& art_name,const std::string& song_id){
	return redis_engine_->AddHashRadomElement(art_name.c_str(),song_id.c_str(),song_id.length());
}

bool RedisDicSerial::GetMusicMapRadom(const std::string& art_name,std::string& song_id){
	char* value;
	size_t value_len = 0;
	bool r = false;
	r = redis_engine_->GetHashRadomElement(art_name.c_str(),&value,&value_len,10);
	if (r){
		song_id.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}

	}else{
		MIG_DEBUG(USER_LEVEL,"GetValue error");
	}
	return r;
}

bool RedisDicSerial::DelMusicMapRadom(const std::string& art_name){
	return redis_engine_->DelHashRadomElement(art_name.c_str());
}

bool RedisDicSerial::GetMoodAndScensId(const std::string& key,std::string& word_id){
	char* value;
	size_t value_len = 0;
	bool r = redis_engine_->GetValue(key.c_str(),key.length(),
		&value,&value_len);
	if (r){
		word_id.assign(value,value_len-1);
		if (value){
			free(value);
			value = NULL;
		}

	}else{
		MIG_DEBUG(USER_LEVEL,"GetValue error[%s]",key.c_str());
	}

	return r;
}

bool RedisDicSerial::GetArtistMoodAndScensNum(const std::string& key,
											  std::string& num){
	char* value;
	size_t value_len = 0;
	bool r = redis_engine_->GetValue(key.c_str(),key.length(),
	  &value,&value_len);
	if (r){
	  num.assign(value,value_len-1);
	  if (value){
		  free(value);
		  value = NULL;
	  }

	}else{
		MIG_DEBUG(USER_LEVEL,"GetValue error[%s]",key.c_str());
	}

	return r;
}

}
