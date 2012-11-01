#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include "client/linux/handler/exception_handler.h"
#include "fcgi_stdio.h"
#include "mig_log.h"
#include "config.h"
#include "storage.h"
#include "mysql_opertion.h"

static bool DumpCallBack(const char* dump_path,const char* minidump_id,
			 void* contect,bool succeeded){

    
    printf("Dump path:%s/%s.dump\n",dump_path,minidump_id);
    MIG_LOG(USER_LEVEL,"Dump path:%s/%s.dump\n",dump_path,minidump_id);
    return succeeded;
}

/////test////////////////////
static int TestMysqlStroage(FileConfig& file){
	MysqlOpertion oper;
	oper.Connections(file.db_list_);
	std::string usrname = "kerry";
	std::string password = "123456";
	int sex = 1;
	int address = 4;
	std::string name = "dong";
	std::string id_cards = "510602198609036815";
	std::string head = "http://img.miglab.com/6815.jpg";
	oper.AddUserInfo(usrname,password,sex,address,name,
						id_cards,head);
	return 0;
}

static int TestMemStoage(FileConfig& file){
    StorageEngine* oper = StorageEngine::Create(IMPL_MEM);
    oper->Connections(file.mem_list_);
    std::string skey = "12345";
    std::string svalue = "dong";
    oper->SetValue(skey.c_str(),skey.length(),svalue.c_str(),svalue.length(),NULL);
    return 0;
   
}

static int TestRedis(FileConfig& file){
	StorageEngine* oper = StorageEngine::Create(IMPL_RADIES);
	oper->Connections(file.redis_list_);
	std::string skey = "dong";
	std::string svalue = "xiang";
	oper->SetValue(skey.c_str(),skey.length(),svalue.c_str(),svalue.length(),NULL);
	std::string slist = "kerry";
	std::string svalue1 = "dong";
	std::string svalue2 = "xiang";
	oper->AddListElement(slist.c_str(),slist.length(),svalue1.c_str(),svalue1.length());
	oper->AddListElement(slist.c_str(),slist.length(),svalue2.c_str(),svalue2.length());
	std::list<std::string> erlist;
	oper->GetListAll(slist.c_str(),slist.length(),erlist);
	return 0;
}
///////////////////////////////////////////////

int main(int agrc,char* argv[]){
    
    google_breakpad::ExceptionHandler eh(".",NULL,DumpCallBack,
	NULL,true);
    std::string path = "./config.xml";
    FileConfig file_config;
    if((!file_config.LoadConfig(path))){
	    	return 1;
	}
	TestRedis(file_config);
    while(FCGI_Accept()>=0){
    	printf("Content-type: text/html\r\n"
	    "\r\n"
	    "<title>FastCGI echo</title>"
	    "<h1>FastCGI echo</h1>\n"
        "Process ID: %d<p>\n"
        , getpid()); 
    }
    
    return 0;
}
