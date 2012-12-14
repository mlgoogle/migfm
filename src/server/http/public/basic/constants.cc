#include "constants.h"
#include <string.h>

namespace base{

const std::string& ConStants::str_empty(){
   static const std::string str_empty;
   return str_empty;
}


void ConStants::fcgi_http_head(){
	char* header = "Content-type: text/html\r\n\r\n";
	write(STDOUT_FILENO,header,strlen(header));
}

const std::string & ConStants::ns_stream() {
	static const std::string ns_stream_("http://etherx.jabber.org/streams");
	return ns_stream_;
}
#if defined (XMPP)
const QName QN_STREAM_FEATURES(true, NS_STREAM, "features");
#endif
}
