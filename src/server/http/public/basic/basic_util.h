#ifndef MIG_FM_PUBLIC_BASIC_BASIC_UTIL_H__
#define MIG_FM_PUBLIC_BASIC_BASIC_UTIL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include "basic/basictypes.h"
namespace base{

class BasicUtil{
public:
    BasicUtil(){}
    virtual ~BasicUtil(){}
public:
    static void GetHttpParamElement(const char* query,const char* name,
                                   std::string& value);
    
    static void ParserIdpPost(const std::string& request_string,std::string& sso_info,
                              std::string& username,std::string& password);
    
    static void ParserSpPost(const std::string& request_string,std::string& samlart,
                              std::string& relay_state);
 
    static void PaserRecordPost(const std::string& request_string,std::string& enter,
    	            std::string& url_code,std::string& content);
    	                        	
    static bool ParserHttpRequest(const std::string& request,
		                  std::map<std::string,std::string >& http_map);
		                          	
    static bool CheckToken(const std::string& request);    
    
    static const std::string& FormatCurrentTime();

    static std::string  GetRawString(std::string str);

    static bool UrlDecode(std::string& content,std::string& out_str);

    static int32 php_htoi(char *s);
};

}


#endif


