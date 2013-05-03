#include "http_engine.h"
#include "fs.h"
#include "thread.h"
#include "thread/base_threadpool.h"
#include "thread/base_threadlock.h"
#include "thread/base_thread.h"
#include "storage/db_serialization.h"
#include "log/mig_log.h"
#include "config/config.h"
#include <sstream>
#include <unistd.h>

static int sock_conn_count = 0;
int main(int agrc,char* argv[]){
    
    bool r = false;
	int32 ret = -1;
	base_threadpool_t* thrp = NULL;
	char buf[102400];
	std::string nameserver = "42.121.126.248:8108";
    std::string path="config.xml";
    config::FileConfig* config = config::FileConfig::GetFileConfig();
    if(config==NULL)
        return 0;
    r = config->LoadConfig(path);
    if(!r)
        return 1;


 	int32 rv = init_threadpool(&thrp,10);
 	MIG_DEBUG(USER_LEVEL,"####rv[%d]##",rv);
 	char* name = "dong";
	for (int i =0;i<10;i++){
		user_addtask(thrp,10,config);
		sleep(3);
	}
	MIG_DEBUG(USER_LEVEL,"#########");
	getchar();
	//sleep(10000);
// 	//stop_threadpool(thrp);
//     r = base_storage::MysqlSerial::Init(config->mysql_db_list_);
// 
// 	filestorage::MFSEngine* engine_ = filestorage::MFSEngine::Create(filestorage::TFS_TYPE);
// 
// 	ret = engine_->Initialize(nameserver.c_str());
// 	if (ret!=1){
// 		MIG_DEBUG(USER_LEVEL,"initialize error");
// 		return 1;
// 	}
	
// 	char* name = new char[64];
// 	int32 name_length = 64;
// 	std::string file_path = "/root/pj/http/tfs/p1382521.mp3";
// 	std::string local_file = "/root/pj/http/tfs/kerry.mp3";
// 	int32 flags = tfs::common::T_DEFAULT;
// 	char* suffix = "mp3";
// 	ret = engine_->SaveFile(name,name_length,file_path.c_str(),
// 				flags,suffix,nameserver.c_str());
// 
// 	MIG_DEBUG(USER_LEVEL,"#####ret[%d]######",ret);
// 	MIG_DEBUG(USER_LEVEL,"###name[%s]####",name);
// 
// 
// 	ret = engine_->FetchFile(local_file.c_str(),name,suffix,nameserver.c_str());
	//}else{
	  //  MIG_DEBUG(USER_LEVEL,"write failed[%d]",ret);
	//}
//   */ //base_threadpool_t          *thrp;
    //init_threadpool(thrp,1);
    //int n = 0;
    //sys_addtask(thrp,0,mi);
    //sleep(10000000);
    //stop_threadpool(thrp);
    return 1;
}
