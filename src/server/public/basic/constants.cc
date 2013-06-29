#include "constants.h"
#include "basic/basic_util.h"
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

const std::string& ConStants::channel_dec(){
	char* out;
	size_t out_len;
	static std::string str_out;
	static const std::string dec = "���Ѿ��л����µ�Ƶ������ǰƵ����-";
	base::BasicUtil::GB2312ToUTF8(dec.c_str(),dec.length(),&out,&out_len);
	str_out.assign(out,out_len);
	if (out){
		delete [] out;
		out = NULL;
	}
	return str_out;
}

#if defined (XMPP)
const QName QN_STREAM_FEATURES(true, NS_STREAM, "features");
#endif
}
