#include <stdio.h>
#include <stdlib.h>
#include <list>
#include "parser_engine.h"
#include "basic/basic_info.h"
#include "basic/basic_util.h"
#include "config/config.h"
#include "storage/db_serialization.h"
#include "xmpp/xmppstanzaparser.h"





int main(int agrc,char* agrv[]){
    
    bool r = false;
    std::string path = "./config.xml";
    std::list<base::MusicUsrInfo> mi_list;
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
    r = base_storage::MysqlSerial::GetMusicInfos(mi_list);
    if(!r)
        return r;
    ParserEngine* engine = ParserEngine::Create();
    
    std::list<base::MusicUsrInfo>::iterator it = mi_list.begin();
    while(it!=mi_list.end()){
        engine->ParserXml((*it).content());
        ++it;
    }
    return 1;
}
