#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include "id_provider.h"
#include "log/mig_log.h"
#include "client/linux/handler/exception_handler.h"

#if defined (FCGI_STD)
#include "fcgi_stdio.h"
#elif defined(FCGI_PLUS)
#include "fcgio.h"
#include "fcgi_config.h"
#endif

#include "log/mig_log.h"
#include "basic/basic_util.h"
static bool DumpCallBack(const char* dump_path,const char* minidump_id,
			 void* contect,bool succeeded){
			 	    
    printf("Dump path:%s/%s.dump\n",dump_path,minidump_id);
    MIG_LOG(USER_LEVEL,"Dump path:%s/%s.dump\n",dump_path,minidump_id);
    return succeeded;
}



#if defined (FCGI_STD)

static void GetRequestMethod(const char* query){

}

static void PostRequestMethod(std::string& content){
    MIG_INFO(USER_LEVEL,"post content [%s]",content.c_str());
    bool r = false;
#if defined (IDP_CHECK)
    std::string sso;
    std::string username;
    std::string password;
    std::string response;
    base::BasicUtil::ParserIdpPost(content,sso,username,password);
    MIG_INFO(USER_LEVEL,"username[%s] password[%s] sso[%s]",
             username.c_str(),password.c_str(),sso.c_str());
   
    r = mig_sso::IDProvider::GetInstance()->SSOCheckUser(sso,username,password,response);
    if(r)
        printf("Content-type: text/html\r\n"
           "\r\n"
           "%s\n"
           ,response.c_str());
#elif defined (SOAP_CHECK)
    std::string soap_response;
    r = mig_sso::IDProvider::GetInstance()->SSOIdpCheck(content,soap_response);
    
    if(r)
        printf("Content-type: text/html\r\n"
           "\r\n"
           "%s\n"
           ,soap_response.c_str());
#endif
    if(!r) 
        printf("Content-type: text/html\r\n"
           "\r\n"
           "check errorn\n");
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
        if(content){
            delete content;
            content = NULL;
        }
    }
} 


static void PutRequestMethod(FCGX_Request * request){

}

static void DeleteRequestMethod(FCGX_Request * request){

}
#endif


int main(int agrc,char* argv[]){
    
    google_breakpad::ExceptionHandler eh(".",NULL,DumpCallBack,NULL,true);
    std::string path = "./sso_config.xml";
   
    std::string content;
    const char* query;
    bool r = false;
    
    r = mig_sso::IDProvider::GetInstance()->InitSSO(path);
    
    MIG_INFO(USER_LEVEL,"init fastcgi id:%d",getpid());
#if defined (FCGI_PLUS)
    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request,0,0);
#endif

     if(!r)
         return r;
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
    char* request_method = "GET";
#else
    char *request_method = getenv("REQUEST_METHOD");
#endif
    char *contentLength = getenv("CONTENT_LENGTH");
    MIG_INFO(USER_LEVEL,"request_method[%s] content_length[%s]type[%s]",request_method,contentLength,getenv("CONTENT_LENGTH"));
    if(strcmp(request_method,"POST")==0){
        //std::cin>>content; 
        int len = strtol(contentLength,NULL,10);
        for(int i = 0;i<len;i++){
            char ch;
            ch=getchar();
            content.append(1,ch);
            //MIG_INFO(USER_LEVEL,"requesr content getchar[%c]",ch);
            //putchar(ch);
        }
        //MIG_INFO(USER_LEVEL,"request content [%s]",content.c_str());
         //MIG_INFO(USER_LEVEL,"request content[%s][%c]",content.c_str(),t);
        //MIG_INFO(USER_LEVEL,"post content [%d]",getenv("CONTENT_LENGTH"));
        PostRequestMethod(content);
        content.clear();
    }
		
    else if(strcmp(request_method,"GET")==0){
#if defined (TEST)
        query = "userid=10001";
#else
        query = getenv("QUERY_STRING");
#endif
        GetRequestMethod(query);
    }
		
    else if(strcmp(request_method,"PUT")==0){
        std::cin>>content;
        PutRequestMethod(content);
    }
		
    else if(strcmp(request_method,"DELETE")==0){
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
    
    MIG_INFO(USER_LEVEL,"deinit fastcgi id:%d",getpid());
    return 0;
}


