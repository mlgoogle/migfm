#ifndef MIG_FM_PUBLIC_BASIC_BASIC_UTIL_H__
#define MIG_FM_PUBLIC_BASIC_BASIC_UTIL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iconv.h>
#include <vector>
#include <assert.h>
#include "zlib.h"
#include "basic/basictypes.h"
#include "basic/md5sum.h"
#include "basic/basic_info.h"
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

    static std::string GetSrcString(std::string str);

    static bool UrlDecode(std::string& content,std::string& out_str);

    static int32 php_htoi(char *s);

	static bool GB2312ToUTF8 (const char *input, size_t inlen, char **output /* free */, size_t *outlen);

	static bool UTF8ToGB2312 (const char *input, size_t inlen, char **output /* free */, size_t *outlen);

	static double CalcGEODistance(double latitude1, double longitude1, double latitude2, double longitude2);

	static int SplitStringChr( const char *str, const char *char_set,
		std::vector<std::string> &out );

	static bool GetUserToken(const std::string& uid,std::string& token);
	
	static bool CheckUserToken(const std::string& uid,const std::string& token);

	static bool GetUserInfo(const std::string& uid,UserInfo& usrinfo);

	static bool ConverNum(const int num,std::string& conver_num);

	static int HttpgzdeCompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata);
};

}


#endif


