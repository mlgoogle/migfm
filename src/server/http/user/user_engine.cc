#include "user_engine.h"
#include "log/mig_log.h"
#include "config/config.h"
#include "storage/db_serialization.h"
#include "storage/dic_serialization.h"
#include "basic/basic_util.h"
#include <sstream>
namespace userinfo{

userinfo::UserInfoEngine* UserInfoEngine::engine_ = NULL;
UserInfoEngine::UserInfoEngine(){

    xml_serialization_.reset(new base::XmlSerialization());
}

UserInfoEngine::~UserInfoEngine(){

}

UserInfoEngine* UserInfoEngine::GetEngine(){
    if(engine_==NULL){
        engine_ = new UserInfoEngine();
    }
    return engine_;
}

void UserInfoEngine::FreeEngine(){
    if(engine_!=NULL){
        delete engine_;
        engine_ = NULL;
    }
}

bool UserInfoEngine::InitEngine(std::string& path){
    bool r = false;
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    if(config==NULL){
        return r;
    }
    r = config->LoadConfig(path);
    if(!r)
    	return r;
    r = base_storage::MysqlSerial::Init(config->mysql_db_list_);
    if(!r)
        return r;
    r = base_storage::MemDicSerial::Init(config->mem_list_);
    if(!r)
        return r;
    r = base_storage::RedisDicSerial::Init(config->redis_list_);
    if(!r)
        return r;
    return r;
}


bool UserInfoEngine::GetUserInfo(const char* query,std::string& result){
    
    //std::string result;
    std::string usr_id;
    int32 sex;
    std::string ext_add;
    std::string street;
    std::string locality;
    std::string regin;
    int32 pcode;
    std::string ctry;
    std::string head;
    std::string birthday;
    std::string nickname;
    std::string username;
    const char* key="username";
    bool r = false;
    std::stringstream mem_key;
    char* mem_value = NULL;
    size_t mem_value_length = 0;
    //parser    
    base::BasicUtil::GetHttpParamElement(query,key,username);

    //usr_id = atol(str_id.c_str());

    mem_key<<username<<"_userinfo";

    //get memcached
    r = base_storage::MemDicSerial::GetString(mem_key.str().c_str(),mem_key.str().length(),
                                              &mem_value,&mem_value_length);
    if(!r){
        r = base_storage::MysqlSerial::GetUserInfo(username,usr_id,sex,ext_add,street,
            locality,regin,pcode,ctry,head,birthday,nickname);

        xml_serialization_->XmlUserInfoSerialization(result,usr_id,username,sex,ext_add,street,
            locality,regin,pcode,ctry,head,birthday,nickname);
   
        //write memcached
        r = base_storage::MemDicSerial::SetString(mem_key.str().c_str(),mem_key.str().length(),
                                result.c_str(),result.length());
        MIG_DEBUG(USER_LEVEL,"write[%d] key[%s] value[%s]",r,mem_key.str().c_str(),
                  result.c_str());
    }else{
         result.assign(mem_value,mem_value_length);
         r = true;
    }

    return r;
}

}
