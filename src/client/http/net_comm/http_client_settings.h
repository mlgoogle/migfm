#ifndef _NET_COMM_HTTP_CLIENT_SETTINGS_H__
#define _NET_COMM_HTTP_CLIENT_SETTINGS_H__
#include <string>
#include "net_comm/http_constants.h"
namespace net_comm{

typedef std::string CryptString;
class HttpClientSettings{

public:
	HttpClientSettings():
	  use_tls_(false),proxy_port_(80),use_proxy_auth_(false){}

	  void set_user(const std::string& user){user_ = user;}
	  void set_host(const std::string& host){host_ = host;}
	  void set_pass(const CryptString& pass){pass_ = pass;}
	  void set_resource(const std::string& resource) {resource_ = resource;}
	  void set_use_tls(bool use_tls) {use_tls_ = use_tls;}
	  void set_proxy_host(std::string& proxy_host){proxy_host_ = proxy_host;}
	  void set_proxy_user(const std::string& user) {proxy_user_ = user;}
	  void set_proxy_pass(const CryptString& pass) {proxy_pass_ = pass;}
	  void set_proxy_port(const int port) {proxy_port_ = port;}

	  const std::string& user() const {return user_;}
	  const std::string& host() const {return host_;}
	  const CryptString& pass() const {return pass_;}
	  const std::string& resource() const {return resource_;}
	  bool use_tls() const {return use_tls_;}
	  int proxy_port() const {return proxy_port_;}
	  const std::string& proxy_host() const {return proxy_host_;}
	  const std::string& proxy_user() const {return proxy_user_;}
	  const CryptString& proxy_pass() const {return proxy_pass_;}

private:
	bool         use_tls_;
	std::string  user_;
	std::string  host_;
	CryptString  pass_;
	std::string  resource_;
	std::string  proxy_host_;
	bool         use_proxy_auth_;
	std::string  proxy_user_;
	CryptString  proxy_pass_;
	int          proxy_port_;

};

}
#endif