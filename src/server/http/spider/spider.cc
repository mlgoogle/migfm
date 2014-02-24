#include "http_engine.h"
#include "thread.h"
#include "thread/base_threadpool.h"
#include "thread/base_threadlock.h"
#include "thread/base_thread.h"
#include "storage/db_serialization.h"
#include "config/config.h"
#include <sstream>
#include <unistd.h>

static int sock_conn_count = 0;
int main(int agrc,char* argv[]){
    
    bool r = false;
    std::string path="config.xml";
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    if(config==NULL)
        return 0;
    r = config->LoadConfig(path);
    if(!r)
        return 1;
    r = base_storage::MysqlSerial::Init(config->mysql_db_list_);

    base_threadpool_t          *thrp;
    //init_threadpool(thrp,1);
    int n = 0;
    for(n = 1154083;n<8999999;n++){
        struct GetMusicInfo* mi = new struct GetMusicInfo;
        mi->count = n;
        spider::SpiderHttpEngine* engine_ = new spider::SpiderHttpEngineImpl();
        std::stringstream url;
        url<<"http://dbfmdb.sinaapp.com/api/songs.php?sids="<<mi->count;
        engine_->RequestHttp(url.str());
        if(mi){delete mi;mi = NULL;}
        if(engine_){delete engine_;engine_=NULL;}
        //sleep(2);
    }
    //sys_addtask(thrp,0,mi);
    //sleep(10000000);
    //stop_threadpool(thrp);
    return 1;
}
