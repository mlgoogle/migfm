#include "constants.h"


namespace base{

const std::string& ConStants::str_empty(){
   static const std::string str_empty;
   return str_empty;
}


void ConStants::fcgi_http_head(){
	char* header = "Content-type: text/html\r\n\r\n";
	write(STDOUT_FILENO,header,strlen(header));
}

}
