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
   
    return mem_engine_->SetValue(ticket,strlen(ticket)+1,
                          os.str().c_str(),os.str().length());
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

}
