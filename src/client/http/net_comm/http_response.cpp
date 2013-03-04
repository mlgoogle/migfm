#include "net_comm/http_response.h"
#include "net_comm/metadata.pb.h"
#include "base/logging.h"
#include "net_comm/http_base.h"
#include "googleurl/src/gurl.h"
#include "curl/curl.h"

#include <locale>
#include <algorithm>

namespace net_comm{

struct CurlContent{
	HttpBase* base_;
	net::type::Metadatas *headers_;
	net_comm::HttpResponse::ContentType *content_;
	int code_;
	int subversion_;
};

static size_t HeaderFunction(void *ptr, size_t size, size_t nmemb, 
							 void *context){
	 CurlContent *curl_content = (CurlContent *)context;
	 char *start, *end;
	 char *p = (char *)ptr;
	 std::string name, value;
	 if(!curl_content->code_) {
		 while(*p && isspace(*p))
			 ++p;
		 int rc = sscanf(p, "HTTP/1.%d %d", &curl_content->subversion_,
			 &curl_content->code_);
		 if(rc != 2)
			 return -1;
		 else
			 return size*nmemb;
	 }
	 while(*p && isspace(*p))
		 ++p;
	 if(!*p)
		 return p-(char*)ptr;
	 start = p;
	 while(*p && (*p != ':'))
		 ++p;
	 if(!*p)
		 return p-(char*)ptr;
	 end = p++;
	 while((end > start) && isspace(*end))
		 end--;
	 name.assign(start, end-start);

	 while(*p && isspace(*p))
		 ++p;
	 if(!*p)
		 return p-(char*)ptr;
	 start = p;

	 end = strchr(p, '\r');
	 if(!end)
		 end = strchr(p, '\n');
	 if(!end)
		 end = strchr(p, '\0');
	 if(!end)
		 return end-(char*)ptr;
	 while((end > start) && isspace(*end))
		 end--;
	 value.assign(start, end+1-start);

	 net::type::Metadatas_Metadata *metadata =
		 curl_content->headers_->add_values();
	 metadata->set_name(name);
	 metadata->set_value(value);
	 return size*nmemb;

}

static size_t ContentFunction(void *ptr, size_t size, size_t nmemb,
							  void *context){
	  CurlContent *curl_content = (CurlContent *)context;
	  if(curl_content->content_->size() <
		  (unsigned int)curl_content->base_->max_content_size()) {
			  curl_content->content_->insert(curl_content->content_->end(), 
				  (char *)ptr, (char *)ptr+std::min(size*nmemb,
				  curl_content->base_->max_content_size() -
				  curl_content->content_->size()));
	  }
	  return size*nmemb;
}


HttpResponse::HttpResponse(HttpBase *http, const GURL &url)
:http_(http)
,url_(url){

	headers_ = new net::type::Metadatas;

}

HttpResponse::~HttpResponse(){
	if (headers_){
		delete headers_;
		headers_ = NULL;
	}
}

const std::string& HttpResponse::GetHeader(std::string &name){
	static const std::string empty = "";
	if (headers_){
		for (int i = 0; i<headers_->values_size();++i){
			if (headers_->values(i).name()==name)
				return headers_->values(i).value();
		}
	}
	return empty;
}

bool HttpResponse::Post(const std::string& post_content){
	CURL* curl = curl_easy_init();
	CURLcode curl_code;
	char curl_error[CURL_ERROR_SIZE];
	bool result = false;
	CurlContent	curl_content;

	if(!curl){
		LOG(WARNING)<<"curl_easy_init error";
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_ERRORBUFFER failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_NOPROGRESS failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_NOSIGNAL failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_FAILONERROR failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	if(http_->use_proxy()){
		curl_code = curl_easy_setopt(curl,CURLOPT_PROXY,
			http_->proxy_host().c_str());
		if(curl_code != CURLE_OK) {
			LOG(WARNING) << "curl_easy_setopt, CURLOPT_PROXY failed: "
				<< curl_easy_strerror(curl_code);
			goto out;
		}

		curl_code = curl_easy_setopt(curl,CURLOPT_PROXYPORT,
			http_->proxy_port());
		if(curl_code != CURLE_OK) {
			LOG(WARNING) << "curl_easy_setopt, CURLOPT_PROXYPORT failed: "
				<< curl_easy_strerror(curl_code);
			goto out;
		}
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_TIMEOUT,http_->user_agent());
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_USERAGENT failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_URL,url_.spec().c_str());
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_URL failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post_content.c_str());
	if (curl_code!=CURLE_OK){
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_POSTFIELDS failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}
	curl_content.base_ = http_;
	curl_content.code_ = 0;
	curl_content.content_ = &content_;
	curl_content.headers_ =headers_;
	curl_content.subversion_ = 0;
	curl_code = curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,HeaderFunction);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_HEADERFUNCTION failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_HEADERDATA,&curl_content);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_HEADERDATA failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,ContentFunction);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_WRITEFUNCTION failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_WRITEDATA,&curl_content);

	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_WRITEDATA failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_perform(curl);
	if(curl_code != CURLE_OK) {
		LOG(ERROR) << "curl_easy_perform failed: " 
			<< curl_easy_strerror(curl_code);
		goto out;
	}
	code_ = curl_content.code_;
	result = true;
out:
	curl_easy_cleanup(curl);
	return result;

}

bool HttpResponse::Get(){
	CURL* curl = curl_easy_init();
	CURLcode curl_code;
	char curl_error[CURL_ERROR_SIZE];
	bool result = false;
	CurlContent	curl_content;

	if(!curl){
		LOG(WARNING)<<"curl_easy_init error";
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_ERRORBUFFER failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_NOPROGRESS failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_NOSIGNAL failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_FAILONERROR failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	if(http_->use_proxy()){
		curl_code = curl_easy_setopt(curl,CURLOPT_PROXY,
									 http_->proxy_host().c_str());
		if(curl_code != CURLE_OK) {
			LOG(WARNING) << "curl_easy_setopt, CURLOPT_PROXY failed: "
				<< curl_easy_strerror(curl_code);
			goto out;
		}

		curl_code = curl_easy_setopt(curl,CURLOPT_PROXYPORT,
									 http_->proxy_port());
		if(curl_code != CURLE_OK) {
			LOG(WARNING) << "curl_easy_setopt, CURLOPT_PROXYPORT failed: "
				<< curl_easy_strerror(curl_code);
			goto out;
		}
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_TIMEOUT,http_->user_agent());
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_USERAGENT failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_URL,url_.spec().c_str());
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_URL failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}


	curl_content.base_ = http_;
	curl_content.code_ = 0;
	curl_content.content_ = &content_;
	curl_content.headers_ =headers_;
	curl_content.subversion_ = 0;
	curl_code = curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,HeaderFunction);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_HEADERFUNCTION failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_HEADERDATA,&curl_content);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_HEADERDATA failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,ContentFunction);
	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_WRITEFUNCTION failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_setopt(curl,CURLOPT_WRITEDATA,&curl_content);

	if(curl_code != CURLE_OK) {
		LOG(WARNING) << "curl_easy_setopt, CURLOPT_WRITEDATA failed: "
			<< curl_easy_strerror(curl_code);
		goto out;
	}

	curl_code = curl_easy_perform(curl);
	if(curl_code != CURLE_OK) {
		LOG(ERROR) << "curl_easy_perform failed: " 
			<< curl_easy_strerror(curl_code);
		goto out;
	}
	code_ = curl_content.code_;
	result = true;
out:
	curl_easy_cleanup(curl);
	return result;
}


}