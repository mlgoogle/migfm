#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "client/linux/handler/exception_handler.h"
#include "fcgio.h"
#include "fcgi_config.h"
#include "mig_log.h"
#include "config.h"
#include "user_mgr.h"

static bool DumpCallBack(const char* dump_path,const char* minidump_id,
			 void* contect,bool succeeded){

    
    printf("Dump path:%s/%s.dump\n",dump_path,minidump_id);
    MIG_LOG(USER_LEVEL,"Dump path:%s/%s.dump\n",dump_path,minidump_id);
    return succeeded;
}

static void GetRequestMethod(FCGX_Request * request){
	
}

static void PostRequestMethod(FCGX_Request * request){
	char * clenstr = FCGX_GetParam("CONTENT_LENGTH", request->envp);
	if(clenstr){
		unsigned long clen = strtol(clenstr, &clenstr, 10);
		char* content = new char[clen];
		std::cin.read(content, clen);
        clen = std::cin.gcount();
		userinfo::UserMgr::GetInstance()->PostUserInfo(content,clen);
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

int main(int agrc,char* argv[]){
    
    google_breakpad::ExceptionHandler eh(".",NULL,DumpCallBack,
	NULL,true);
	FCGX_Request request;
    std::string path = "./config.xml";
    base::FileConfig file_config;
    FCGX_Init();
    FCGX_InitRequest(&request,0,0);
    if((!file_config.LoadConfig(path))){
	    	return 1;
	}
	std::string test = "<message type=\'chat\' id=\'v1\' to=\'v1@gmail.com\'><active xmlns=\'http://jabber.org/protocol/chatstates\'/></message>";
    userinfo::UserMgr::GetInstance()->PostUserInfo(test.c_str(),test.length());
    while(FCGX_Accept_r(&request)==0){
    	/*printf("Content-type: text/html\r\n"
	    "\r\n"
	    "<title>FastCGI echo</title>"
	    "<h1>FastCGI echo</h1>\n"
        "Process ID: %d<p>\n"
        , getpid());*/
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
		char *request_method = FCGX_GetParam("REQUEST_METHOD",request.envp);
		if(strcmp(request_method,"POST")==0)
			PostRequestMethod(&request);
		else if(strcmp(request_method,"GET")==0)
			GetRequestMethod(&request);
		else if(strcmp(request_method,"PUT")==0)
			PutRequestMethod(&request);
		else if(strcmp(request_method,"DELETE")==0)
			DeleteRequestMethod(&request);
    }
	userinfo::UserMgr::FreeInstance();
    return 0;
}
