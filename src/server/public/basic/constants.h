#ifndef _MIG_FM_CONSTANTS_H__
#define _MIG_FM_CONSTANTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define STR_EMPTY       base::ConStants::str_empty()
#define DEFAULT_HEAD    base::ConStants::str_head()
#define STR_ZONE        base::ConStants::str_zone()
#define FCGI_HTTP_HEAD  base::ConStants::fcgi_http_head()
#define NS_STREAM       base::ConStants::ns_stream()

namespace base{

enum MSG_TYPE{
	CONTENT = 1,
	ADDFRINED = 2,
	SENDSONG = 3,
	HALLO = 4,
};
	
class ConStants{

public:
    static const std::string& str_empty();
	static const std::string& str_head();
	static const std::string& str_zone();
    static void fcgi_http_head();
    static const std::string& ns_stream();
	static const std::string& channel_dec();

	static void get_result_msg(std::string& status,std::string& msg,
		                       std::string& result,std::string& out_str);
};

}
#endif
