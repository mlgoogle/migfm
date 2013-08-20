#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
//#include "client/linux/handler/exception_handler.h"

#if defined (FCGI_STD)
#include "fcgi_stdio.h"
#elif defined(FCGI_PLUS)
#include "fcgio.h"
#include "fcgi_config.h"
#endif

#include "log/mig_log.h"
#include "config/config.h"
#include "basic/basic_util.h"
#include "comm/core_interface.h"

static bool DumpCallBack(const char* dump_path,const char* minidump_id,
			 void* contect,bool succeeded){
			 	    
    printf("Dump path:%s/%s.dump\n",dump_path,minidump_id);
    MIG_LOG(USER_LEVEL,"Dump path:%s/%s.dump\n",dump_path,minidump_id);
    return succeeded;
}

static void test_net_ipc(std::string& respone){
	std::string content;
	int flag;
	int code;
	content ="type=regedit&username=flaght&password=123456&nickname=kerry&source=1\n";
	MIG_DEBUG(USER_LEVEL,"%s",content.c_str());
	net::core_get(0,content.c_str(),content.length(),respone,flag,code);
}


#if defined (FCGI_STD)
static void GetRequestMethod(const char* query){

	std::string content;
	std::string respone;
	int flag;
	int code;
	bool r = false;
	content.assign(query);
	content.append("&type=gettypesongs\n");
	MIG_INFO(USER_LEVEL,"%s",content.c_str());
	r = net::core_get(0,content.c_str(),content.length(),
		respone,flag,code);
	MIG_INFO(USER_LEVEL,"%s",respone.c_str());
	if (!respone.empty())
		printf("Content-type: text/html\r\n"
		"\r\n"
		"%s",respone.c_str());
}

static void PostRequestMethod(std::string& content){
  std::string respone;
  int flag;
  int code;
  bool r = false;
  content.append("&type=word\n");
  MIG_DEBUG(USER_LEVEL,"%s",content.c_str());
  r = net::core_get(0,content.c_str(),content.length(),
	            respone,flag,code);
  MIG_DEBUG(USER_LEVEL,"r[%d] response[%s]",r,respone.c_str());
  if (!respone.empty()&&r)
	printf("Content-type: text/html\r\n"
		  "\r\n"
		"%s",respone.c_str());
}

static void PutRequestMethod(std::string& content){

}

static void DeleteRequestMethod(std::string& content){

}
#elif defined (FCGI_PLUS)

static void GetRequestMethod(FCGX_Request * request){
	
}

static void PostRequestMethod(FCGX_Request * request){
    char * clenstr = FCGX_GetParam("CONTENT_LENGTH", request->envp);
    if(clenstr){
        unsigned long clen = strtol(clenstr, &clenstr, 10);
        char* content = new char[clen];
        std::cin.read(content, clen);
        clen = std::cin.gcount();
       // UserMgr::GetInstance()->PostUserInfo(content,clen);
        if(content){
            delete content;
            content = NULL;
        }
    }
    return;
}

static void PutRequestMethod(FCGX_Request * request){

}

static void DeleteRequestMethod(FCGX_Request * request){

}
#endif

int main(int agrc,char* argv[]){
    
    //google_breakpad::ExceptionHandler eh(".",NULL,DumpCallBack,NULL,true);
    std::string path = "./config.xml";
	std::string ipc_conn = "/var/www/tmp/corefile";
    //config::FileConfig file_config;
    std::string content;
    const char* query;
    bool r = false;
    MIG_INFO(USER_LEVEL,"init fastcgi id:%d",getpid());
	net::core_connect_ipc(ipc_conn.c_str());
	MIG_DEBUG(USER_LEVEL,"path[%s]",ipc_conn.c_str());

	//test_net_ipc(content);
#if defined (FCGI_PLUS)
    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request,0,0);
#endif
#if defined (FCGI_STD)
    while(FCGI_Accept()==0){
#elif defined (FCGI_PLUS)
    while(FCGX_Accept_r(&request)==0){
#endif

#if defined (FCGI_PLUS)
    	fcgi_streambuf cin_fcgi_streambuf(request.in);
    	fcgi_streambuf cout_fcgi_streambuf(request.out);
    	fcgi_streambuf cerr_fcgi_streambuf(request.err);
    	
#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
        std::cin  = &cin_fcgi_streambuf;
        std::cout = &cout_fcgi_streambuf;
        std::cerr = &cerr_fcgi_streambuf;
#else
        std::cin.rdbuf(&cin_fcgi_streambuf);
        std::cout.rdbuf(&cout_fcgi_streambuf);
        std::cerr.rdbuf(&cerr_fcgi_streambuf);
#endif

#endif

#if defined (FCGI_STD)
    
#if defined (TEST)
    char* request_method = "POST";
#else
    char *request_method = getenv("REQUEST_METHOD");
#endif 
    char *contentLength = getenv("CONTENT_LENGTH");
    //MIG_INFO(USER_LEVEL,"request_method[%s] content_length[%s]type[%s]",request_method,contentLength,getenv("CONTENT_LENGTH"));
    if(strcmp(request_method,"POST")==0){
        int len = strtol(contentLength,NULL,10);
        for(int i = 0;i<len;i++){
            char ch;
            ch=getchar();
            content.append(1,ch);
        }
        PostRequestMethod(content);
        content.clear();
    }else if(strcmp(request_method,"GET")==0){
        query = getenv("QUERY_STRING");
        GetRequestMethod(query);
    }else if(strcmp(request_method,"PUT")==0){
        std::cin>>content;
        PutRequestMethod(content);
    }else if(strcmp(request_method,"DELETE")==0){
        std::cin>>content;
        DeleteRequestMethod(content);
    }
    
#elif defined (FCGI_PLUS)
    char *request_method = FCGX_GetParam("REQUEST_METHOD",request.envp);
	if(strcmp(request_method,"POST")==0)
		PostRequestMethod(&request);
	else if(strcmp(request_method,"GET")==0)
		GetRequestMethod(&request);
	else if(strcmp(request_method,"PUT")==0)
		PutRequestMethod(&request);
	else if(strcmp(request_method,"DELETE")==0)
		DeleteRequestMethod(&request);
#endif
    }
    
	net::core_close();
    MIG_INFO(USER_LEVEL,"deinit fastcgi id:%d",getpid());
    //UserMgr::FreeInstance();
    return 0;
}
