#ifndef _MIG_FM_CONSTANTS_H__
#define _MIG_FM_CONSTANTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#define STR_EMPTY  base::ConStants::str_empty()
#define FCGI_HTTP_HEAD  base::ConStants::fcgi_http_head()

namespace base{
	
class ConStants{

public:
    static const std::string& str_empty();
    static void fcgi_http_head();
};

}
#endif
