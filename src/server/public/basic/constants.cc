#include "constants.h"
#include "basic/basic_util.h"
#include <string.h>
#include <sstream>

namespace base{

const std::string& ConStants::str_empty(){
   static const std::string str_empty;
   return str_empty;
}

const std::string& ConStants::str_head(){
	static const std::string str_head = "http://fm.miglab.com/default.jpg";
	return str_head;
}

const std::string& ConStants::str_zone(){
	static const std::string str_zone = "0";
	return str_zone;
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
	static const std::string dec = "您已经切换到新的频道，当前频道是-";
	base::BasicUtil::GB2312ToUTF8(dec.c_str(),dec.length(),&out,&out_len);
	str_out.assign(out,out_len);
	if (out){
		delete [] out;
		out = NULL;
	}
	return str_out;
}

void ConStants::get_result_msg(std::string &status, 
							   std::string &msg, 
							   std::string &result,
							   std::string &out_str){
    char* out;
    size_t out_len;
	std::stringstream os;
	os<<"{\"status\":"<<status.c_str()<<",\"msg\":"
		<<msg.c_str()<<",\"result\":{"<<result.c_str()
		<<"}";
	base::BasicUtil::GB2312ToUTF8(os.str().c_str(),os.str().length(),
		                          &out,&out_len);
	out_str.assign(out,out_len);
}

#if defined (XMPP)
const QName QN_STREAM_FEATURES(true, NS_STREAM, "features");
#endif
}
