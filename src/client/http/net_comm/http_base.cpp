#include "net_comm/http_base.h"

#include "net_comm/schema.pb.h"
#include "net_comm/http_response.h"
#include "googleurl/src/gurl.h"
#include "base/logging.h"
#include "base/values.h"
#include "base/scoped_ptr.h"
#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "net_comm/schema_output.h"
#include "net_comm/content.pb.h"
#include "net_comm/http_headers.h"

namespace net_comm {
	
HttpBase::HttpBase(void)
:proxy_port_(8080)
,use_proxy_(false)
,timeout_(10)
,max_content_size_(1024*1024)
,max_delays_count_(3)
,max_thread_per_host_(1)
,server_delay_(100011)
,check_blocking_(false)
,allow_forbidden_(false){

}

HttpBase::~HttpBase(){}



void HttpBase::__RegisterConfs(Configuration* conf){

}

bool HttpBase::PostSchemaOutPut(const std::string &urlstring, 
								const std::string &post_content, 
								net_comm::SchemaOutPut &output){
		GURL url(urlstring);
		if (!url.is_valid()||url.is_empty()){
			LOG(ERROR)<<"url("<<urlstring<<") error";
			return false;
		}

		schema::type::SchemaStatus* status = new schema::type::SchemaStatus();
		if(check_blocking_){
			status->set_code(schema::type::SchemaStatus::WOULDBLOCK);
			status->set_last_modified(0ull);
			status->add_args(urlstring.c_str(),urlstring.length());
			output.set_content(NULL);
			output.set_staus(status);
			return true;
		}

		scoped_ptr<net_comm::Response> response(
			PostResponse(url,post_content,output,false));

		if (response.get()==NULL){
			delete status;
			return true;
		}

		int code = response->GetCode();
		const net_comm::HttpResponse::ContentType& content = response->GetContent();

		schema::type::Content* c = new schema::type::Content();
		c->set_url(url.spec());
		c->set_base(url.spec());

		if (content.size()>0)
			c->set_content(&content.at(0),content.size());
		std::string content_type = net::HttpHeaders::kContentType;
		c->set_content_type(response->GetHeader(content_type));

		net::type::Metadatas *metadata = c->mutable_metadata();
		metadata->CopyFrom(*response->GetHeaders());

		if(code == 200) { // success
			status->set_code(schema::type::SchemaStatus::SUCCESS);
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		else if (code == 410) { // page is gone
			status->set_code(schema::type::SchemaStatus::GONE);
			status->add_args(base::StringPrintf("Http: %d url=%s", code,
				url.spec().c_str()));
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		else if (code >= 300 && code < 400) { // handle redirect
			std::string location_name = net::HttpHeaders::kLocation;
			const std::string& location = response->GetHeader(
				location_name);
			std::string locale;
			if(location.size() == 0) {
				location_name = net::HttpHeaders::kLocationAlias;
				const std::string& location_alias =
					response->GetHeader(location_name);
				if(location_alias.size() == 0) {
					locale = "";
				}
				else
					locale = location_alias;
			}
			else
				locale = location;

			GURL location_url = url.Resolve(locale);
			schema::type::SchemaStatus::Status status_code;
			switch (code) {
				case 300: // multiple choices, preferred value in Location.
				status_code = schema::type::SchemaStatus::MOVED;
				break;
				case 301: // moved permanently
				case 305: // use proxy (Location is URL of proxy)
				status_code = schema::type::SchemaStatus::MOVED;
				break;
				case 302: // found (temporarily moved)
				case 303: // see other (redirect after POST)
				case 307: // temporary redirect
				status_code = schema::type::SchemaStatus::TEMP_MOVED;
				break;
				case 304:
				status_code = schema::type::SchemaStatus::NOTMODIFIED;
				break;
				default:
				status_code = schema::type::SchemaStatus::MOVED;
				break;
			}

			status->set_code(status_code);
			status->add_args(location_url.spec());
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		else if (code == 400) { // bad request, mark as GONE
			status->set_code(schema::type::SchemaStatus::GONE);
			status->add_args(url.spec());
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		else if (code == 401) {
			// requests authorization, but no valid auth provided.
			status->set_code(schema::type::SchemaStatus::ACCESS_DENIED);
			status->add_args("Authentication required: " + urlstring);
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		else if (code == 404) { // not found
			status->set_code(schema::type::SchemaStatus::NOTFOUND);
			status->add_args(url.spec());
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		else if(code == 410) { // permanently GONE
			status->set_code(schema::type::SchemaStatus::GONE);
			status->add_args(url.spec());
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		else {
			status->set_code(schema::type::SchemaStatus::EXCEPTION);
			status->add_args(base::StringPrintf("Http: %d url=%s", code,
				url.spec().c_str()));
			output.set_content(c);
			output.set_staus(status);
			return true;
		}
		NOTREACHED();
		return true;

}

bool HttpBase::GetSchemaOutPut(const std::string& urlstring, 
							   net_comm::SchemaOutPut& output){
	GURL url(urlstring);
	if (!url.is_valid()||url.is_empty()){
		LOG(ERROR)<<"url("<<urlstring<<") error";
		return false;
	}

	schema::type::SchemaStatus* status = new schema::type::SchemaStatus();
	if(check_blocking_){
		status->set_code(schema::type::SchemaStatus::WOULDBLOCK);
		status->set_last_modified(0ull);
		status->add_args(urlstring.c_str(),urlstring.length());
		output.set_content(NULL);
		output.set_staus(status);
		return true;
	}

	scoped_ptr<net_comm::Response> response(
		GetResponse(url,output,false));
	
	if (response.get()==NULL){
		delete status;
		return true;
	}

	int code = response->GetCode();
	const net_comm::HttpResponse::ContentType& content = response->GetContent();

	schema::type::Content* c = new schema::type::Content();
	c->set_url(url.spec());
	c->set_base(url.spec());

	if (content.size()>0)
		c->set_content(&content.at(0),content.size());
	std::string content_type = net::HttpHeaders::kContentType;
	c->set_content_type(response->GetHeader(content_type));

	net::type::Metadatas *metadata = c->mutable_metadata();
	metadata->CopyFrom(*response->GetHeaders());

	if(code == 200) { // success
		status->set_code(schema::type::SchemaStatus::SUCCESS);
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	else if (code == 410) { // page is gone
		status->set_code(schema::type::SchemaStatus::GONE);
		status->add_args(base::StringPrintf("Http: %d url=%s", code,
			url.spec().c_str()));
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	else if (code >= 300 && code < 400) { // handle redirect
		std::string location_name = net::HttpHeaders::kLocation;
		const std::string& location = response->GetHeader(
			location_name);
		std::string locale;
		if(location.size() == 0) {
			location_name = net::HttpHeaders::kLocationAlias;
			const std::string& location_alias =
				response->GetHeader(location_name);
			if(location_alias.size() == 0) {
				locale = "";
			}
			else
				locale = location_alias;
		}
		else
			locale = location;

		GURL location_url = url.Resolve(locale);
		schema::type::SchemaStatus::Status status_code;
		switch (code) {
			case 300: // multiple choices, preferred value in Location.
				status_code = schema::type::SchemaStatus::MOVED;
				break;
			case 301: // moved permanently
			case 305: // use proxy (Location is URL of proxy)
				status_code = schema::type::SchemaStatus::MOVED;
				break;
			case 302: // found (temporarily moved)
			case 303: // see other (redirect after POST)
			case 307: // temporary redirect
				status_code = schema::type::SchemaStatus::TEMP_MOVED;
				break;
			case 304:
				status_code = schema::type::SchemaStatus::NOTMODIFIED;
				break;
			default:
				status_code = schema::type::SchemaStatus::MOVED;
				break;
		}

		status->set_code(status_code);
		status->add_args(location_url.spec());
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	else if (code == 400) { // bad request, mark as GONE
		status->set_code(schema::type::SchemaStatus::GONE);
		status->add_args(url.spec());
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	else if (code == 401) {
		// requests authorization, but no valid auth provided.
		status->set_code(schema::type::SchemaStatus::ACCESS_DENIED);
		status->add_args("Authentication required: " + urlstring);
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	else if (code == 404) { // not found
		status->set_code(schema::type::SchemaStatus::NOTFOUND);
		status->add_args(url.spec());
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	else if(code == 410) { // permanently GONE
		status->set_code(schema::type::SchemaStatus::GONE);
		status->add_args(url.spec());
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	else {
		status->set_code(schema::type::SchemaStatus::EXCEPTION);
		status->add_args(base::StringPrintf("Http: %d url=%s", code,
			url.spec().c_str()));
		output.set_content(c);
		output.set_staus(status);
		return true;
	}
	NOTREACHED();
	return true;
}

net_comm::Response* HttpBase::GetResponse(const GURL& url,
										  net_comm::SchemaOutPut& output,
										  bool follow_redirects){
	net_comm::Response* rsp = new net_comm::HttpResponse(this,url);
	rsp->Get();
	return rsp;
	
}

net_comm::Response* HttpBase::PostResponse(const GURL& url, 
										   const std::string& post_content, 
										   net_comm::SchemaOutPut& output, 
										   bool follow_redirects){
	net_comm::Response* rsp = new net_comm::HttpResponse(this,url);
    rsp->Post(post_content);
	return rsp;
}

}