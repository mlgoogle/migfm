#include "basic_util.h"
#include "basic/basictypes.h"
#include "storage/dic_serialization.h"
#include "basic/basic_util.h"
#include <sstream>
namespace base{

 void BasicUtil::GetHttpParamElement(const char* query,const char* name,
                         std::string& value){

    char* str = const_cast<char*>(query);
    char* format_str = str;
    int32 flag = 0;
    while(*str!='\0'){
       if(*str == '='){
           size_t str_name_len = str - format_str;
           std::string str_name(format_str,str_name_len);
           if(strcmp(str_name.c_str(),name)==0){
               flag = 1;
               format_str = str;
           }
       }else if(*str=='&'&&flag){
           size_t str_value_len = str - format_str;
           value.assign(format_str+1,str_value_len);
           flag = 0;
           format_str = str;
       }
       str++;
    }
    if(flag){
      size_t str_value_len = str - format_str;
      value.assign(format_str+1,str_value_len);
    }
}


void BasicUtil::PaserRecordPost(const std::string& request_string,std::string& enter,
    	                          std::string& url_code,std::string& content){
    std::map<std::string,std::string> http_map;
    std::map<std::string,std::string>::iterator it;
    BasicUtil::ParserHttpRequest(request_string,http_map);
    if(http_map.size()<=0)
     	return;
    it = http_map.find("enter");
    if(it!=http_map.end())
    	enter = it->second;
    it = http_map.find("urlcode");
    if(it!=http_map.end())
        url_code = it->second;
    it = http_map.find("content");
    if(it!=http_map.end())
        content = it->second; 
}

bool BasicUtil::ParserHttpRequest(const std::string& request,
		                  std::map<std::string,std::string >& http_map){
    std::string request_str = request;
    while (request_str.length()!=0){
        int32 start_pos = request_str.find("=");
	    int32 end_pos = request_str.find("&")==std::string::nops?request.length():request_str.find("&");
        std::string key = request_str.substr(0,start_pos);
        std::string value = request_str.substr(key.length()+1,end_pos - key.length()-1);
	    http_map[key] = value;
	    if (request_str.find("&")!=std::string::nops)
	        request_str = request_str.substr(end_pos+1,request_str.length());
	    else
            request_str.clear();
    }
    return true;
}

void BasicUtil::ParserIdpPost(const std::string& request_string,std::string& sso_info,
                              std::string& username,std::string& password){
   
    int32 start_pos = request_string.find("username");
    int32 end_pos = request_string.find("&");
    if((start_pos==std::string::nops)||(end_pos==std::string::nops))
    	return;
    username = request_string.substr(start_pos+9,end_pos-9);
    
    std::string temp_str;
    temp_str = request_string.substr(end_pos+1,request_string.length());
    
    start_pos = temp_str.find("password");
    end_pos = temp_str.find("&");
  
    if((start_pos==std::string::nops)||(end_pos==std::string::nops))
    	return;
    	
    password = temp_str.substr(start_pos+9,end_pos-9);
   
    sso_info = temp_str.substr(end_pos+1,request_string.length());
}

void BasicUtil::ParserSpPost(const std::string& request_string,std::string& samlart,std::string& relay_state){
    int32 start_pos = request_string.find("SAMLart");
    int32 end_pos =request_string.find("&");
    
    if((start_pos==std::string::nops)||(end_pos==std::string::nops))
    	return;
    	
    samlart = request_string.substr(start_pos+8,end_pos-8);

    std::string temp_str;
    temp_str = request_string.substr(end_pos+1,request_string.length());

    start_pos = temp_str.find("RelayState");
    end_pos = temp_str.find("&");
    
    if((start_pos==std::string::nops)||(end_pos==std::string::nops))
    	return;
    	
    relay_state = temp_str.substr(start_pos+11,end_pos-11);
}


bool BasicUtil::CheckToken(const std::string& request){
     bool r =false;
     char* mem_value = NULL;
     size_t mem_value_length = 0;
     int32 token_pos = request.find("token");
    if((token_pos==std::string::nops))
    	return false;
     std::string temp_str = request.substr(token_pos,request.length());
     int32 and_pos = temp_str.find("&");
     int32 end_pos = and_pos>0?(and_pos-6):(request.length()-6);
     
     if((and_pos==std::string::nops)||(end_pos==std::string::nops))
    	return;
    	
     std::string token = temp_str.substr(6,end_pos);
     r = base_storage::MemDicSerial::GetString(token.c_str(),token.length(),
                                                &mem_value,&mem_value_length);
     return r;
}

const std::string& BasicUtil::FormatCurrentTime(){
    std::stringstream os;
    time_t current = time(NULL);
    struct tm* local = localtime(&current);
    os<<local->tm_year+1900<<"-"<<local->tm_mon+1<<"-"
      <<local->tm_mday<<" "<<local->tm_hour<<":"
      <<local->tm_min<<":"<<local->tm_sec;

    return os.str();
}

std::string BasicUtil::GetRawString(std::string str){
    std::ostringstream out;
    //out<<'';
    out<<std::hex;
    for(std::string::const_iterator it = str.begin();
            it!=str.end();++it){
    
        out<<"\\\\x"<<(static_cast<short>(*it)&0xff);
    }

    //out<<'\"';
    return out.str();
}

bool BasicUtil::UrlDecode(std::string& content,std::string& out_str){

    char  const *in_str = content.c_str();
    int32 in_str_len = strlen(in_str);
    int32 out_str_len = 0;
    char *str;

    str = strdup(in_str);
    char *dest = str;
    char *data = str;

    while (in_str_len--){
        if (*data == '+'){
            *dest = ' ';
        }
        else if (*data == '%' && in_str_len >= 2 && isxdigit((int) *(data + 1))
                                && isxdigit((int) *(data + 2))){
            *dest = (char) php_htoi(data + 1);
            data += 2;
            in_str_len -= 2;
        } else{
            *dest = *data;
        }
            data++;
            dest++;
    }
    *dest = '\0';
    out_str_len =  dest - str;
    out_str = str;
    free(str);
    return true;
}

int32 BasicUtil::php_htoi(char *s){
    int32 value;
    int32 c;
             
    c = ((unsigned char *)s)[0];
    if (isupper(c))
        c = tolower(c);
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
                         
    c = ((unsigned char *)s)[1];
    if (isupper(c))
        c = tolower(c);
    
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
     
    return (value);
}

}
