#include "record_engine.h"
#include <sstream>
#include "log/mig_log.h"
#include "config/config.h"
#include "storage/db_serialization.h"
#include "storage/dic_serialization.h"
#include "basic/basic_util.h"
#include "json/json.h"

namespace record{

record::RecordEngine* RecordEngine::engine_ = NULL;
RecordEngine::RecordEngine(){

}

RecordEngine::~RecordEngine(){

}

RecordEngine* RecordEngine::GetEngine(){
    if(engine_==NULL){
        engine_ = new RecordEngine();
    }
    return engine_;
}

void RecordEngine::FreeEngine(){
    if(engine_!=NULL){
        delete engine_;
        engine_ = NULL;
    }
}

bool RecordEngine::InitEngine(std::string& path){
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


bool RecordEngine::Recording(std::string& content){
    Json::Reader reader;
    Json::Value root;
    std::string name;
    std::string phone;
    std::string enter;
    std::stringstream os;
    bool r = false;
    MIG_DEBUG(USER_LEVEL,"\n\ncontent[%s]\n",content.c_str());
    //os<<"<info type=\"set\" from=\"flaght@gmail.com\">";
    r = reader.parse(content.c_str(),root);
    if(!r)
        return r;

    name = root["name"].asString();
    phone = root["phone"].asString();
    enter = root["enter"].asString();
    os<<"<info type=\"set\"  from=\""<<name<<"\">";
    int32 music_infos_size = root["music"].size();

    Json::Value music = root["music"];
    for(int i = 0;i<music_infos_size;i++){
        std::string song_name = music[i]["name"].asString();
        std::string singer = music[i]["singer"].asString();
        os<<"<music name=\""<<base::BasicUtil::GetRawString(song_name)<<"\" singer=\""
          <<base::BasicUtil::GetRawString(singer)<<"\" />";
    }

    os<<"</info>";
   MIG_DEBUG(USER_LEVEL,"xml[%s]\n\n",os.str().c_str());
   MIG_DEBUG(USER_LEVEL,"===========\n");
   //write data
   base_storage::MysqlSerial::SetMusicInfo(name,phone,enter,os.str());
   return true;
}

}
