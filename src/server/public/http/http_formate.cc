/*
 * http_formate.cc
 *
 *  Created on: 2015年4月22日
 *      Author: pro
 */

#include "http_formate.h"
#include "log/mig_log.h"

namespace http{

void HttpFormate::SetHttpGetMethod(int32 type,const char* query,
		std::string& content){
	std::stringstream os;
	int code;
	int flag;
	bool r = false;
	char* addr = getenv("REMOTE_ADDR");
	os<<std::string(query)<<"&remote_addr="<<addr<<"&type="<<type<<"\n";
	content = os.str();
	MIG_DEBUG(USER_LEVEL,"%s",content.c_str());
}

void HttpFormate::SetHttpPostMethod(const int32 type,const char* data,
			std::string& content){

	std::string respone;
	int flag;
	int code;
	std::stringstream os;
	bool r = false;
	//若CONTENT_TYPE非application/x-www-form-urlencode 直接pass掉
	char* content_type = getenv("CONTENT_TYPE");
#if defined (CHECK_HEADER)
	if(strcmp(content_type,"application/x-www-form-urlencoded")!=0)
		return;
#endif
	char* addr = getenv("REMOTE_ADDR");
	os<<std::string(data)<<"&remote_addr="<<addr<<"&type="<<type<<"\n";
	content = os.str();
}

void HttpFormate::SetHttpResponse(const std::string& response){
	MIG_DEBUG(USER_LEVEL,"%s",response.c_str());
	if (!response.empty())
		printf("Content-type: application/json;charset=utf-8\r\n"
			  "\r\n"
			"%s",response.c_str());
}

}

