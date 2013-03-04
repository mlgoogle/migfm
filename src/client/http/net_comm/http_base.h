#ifndef _NET_COMM_HTTP_BASE_H__
#define _NET_COMM_HTTP_BASE_H__

#include <string>
#include "base/basictypes.h"
#include "net_comm/export.h"

namespace net_comm{
	class Response;
}

namespace net_comm{
	class Configuration;
}

namespace net_comm{
	class SchemaOutPut;
}
class GURL;

namespace net_comm{

class PE_EXPORT HttpBase{
public:
	HttpBase(void);
	~HttpBase(void);

	virtual bool GetSchemaOutPut(const std::string& url,
								net_comm::SchemaOutPut& output);

	virtual bool PostSchemaOutPut(const std::string& urlstring,
								  const std::string& post_content,
								  net_comm::SchemaOutPut& output);
	//attributes

	const std::string& proxy_host(void){return proxy_host_;}
	int proxy_port(void){return proxy_port_;}
	bool use_proxy(void) {return use_proxy_;}
	int timeout(void) {return timeout_;}
	int max_content_size(void) {return max_content_size_;}
	int max_delays_count(void) {return max_delays_count_;}
	int max_thread_per_host(void) {return max_thread_per_host_;}
	int64 server_delay(void) {return server_delay_;}
	std::string& user_agent(void) {return user_agent_;}
	std::string& accept_lang(void) {return accept_lang_;}
	bool check_blocking(void) {return check_blocking_;}
	bool allow_forbidden(void) {return allow_forbidden_;}

protected:
	void __RegisterConfs(Configuration* conf);

	virtual net_comm::Response* GetResponse(const GURL& url,
		net_comm::SchemaOutPut& output,
		bool follow_redirects);

	virtual net_comm::Response* PostResponse(const GURL& url,
		const std::string& post_content,
		net_comm::SchemaOutPut& output,
		bool follow_redirects);

protected:
	std::string proxy_host_;

	int proxy_port_;

	bool use_proxy_;

	int timeout_;

	int max_content_size_;

	int max_delays_count_;

	int max_thread_per_host_;

	int64 server_delay_;

	std::string user_agent_;

	std::string accept_lang_;

	bool check_blocking_;

	bool allow_forbidden_;
};

}
#endif