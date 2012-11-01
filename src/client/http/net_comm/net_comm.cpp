// net_comm.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include "client/windows/handler/exception_handler.h"
#include "googleurl\src\gurl.h"
#include "base\scoped_ptr.h"
#include "base/logging.h"
#include "net_comm/interface.h"
#include "net_comm/wrapper_http.h"
// #include "net_comm/schema_output.h"
// #include "net_comm/http_base.h"

// #ifdef _DEBUG
// #pragma comment(lib,"../lib/debug/breakpad_handler.lib")
// #else
// #pragma comment(lib,"../lib/relase/breakpad_handler.lib")
// #endif

typedef void (_cdecl* hwatcherInit)();
typedef void (_cdecl* hwatcherDump)();
typedef void (_cdecl* hsetlogname)(char* name);

static bool callback(const wchar_t *dump_path, const wchar_t *id,
					 void *context, EXCEPTION_POINTERS *exinfo,
					 MDRawAssertionInfo *assertion,
					 bool successed)
{

	return successed;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{

 	//google_breakpad::ExceptionHandler eh(L".",
 	//	NULL, callback, NULL,
 	//	google_breakpad::ExceptionHandler::HANDLER_ALL);
//  #if _DEBUG
//  	HMODULE hmem = LoadLibrary(L"memory_watcher_purify.dll");
//  	if (hmem)
//  	{
//  		hwatcherDump watcherdunp = (hwatcherDump)GetProcAddress(hmem,"WatcherDump");
//  		hwatcherInit wathcerInit = (hwatcherInit)GetProcAddress(hmem,"WatcherInit");
//  		hsetlogname setlogname = (hsetlogname)GetProcAddress(hmem,"SetLogName");
//  		wathcerInit();
//  		atexit(watcherdunp);
//  		setlogname("net_comm");
//  	}
//  #endif

//  	std::string url;
//  	net_comm::SchemaOutPut output;
//  	scoped_ptr<net_comm::HttpBase>  http_base;
//  	http_base->GetSchemaOutPut(url,output);
    return TRUE;
}

/*static net_comm::HttpBase* CreateObjectHttp(void){
	net_comm::HttpBase* http_base = new net_comm::HttpBase;
	return http_base;
}*/

int NET_Export MigGetHttpTest(){
  	/*std::string url = "http://60.191.220.135/cgi-bin/echo.fcgi";
   	net_comm::SchemaOutPut output;
   	scoped_ptr<net_comm::HttpBase>  http_base;
	http_base.reset(CreateObjectHttp());
   	http_base->GetSchemaOutPut(url,output);
	LOG(ERROR)<<output.content()->content().c_str();*/
	return 0;
}

int NET_Export MigInit(std::string& host,std::string& entry){

// 	std::string hostname = "app.miglab.com";
// 	std::string entry = "miglab";
	net_comm::WrapperHttp::GetInstance()->Init(entry,host);
	return 1;
}


int NET_Export MigUsrLogin(std::string& username,std::string& passwd,std::string& host){
	net_comm::WrapperHttp::GetInstance()->UserLogin(username,passwd,host);
	return 1;
}


