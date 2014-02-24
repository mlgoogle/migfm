#include "http_response.h"

namespace mig_lbs {

struct CurlContent{
	std::map<std::string,std::string>   headers_;
	std::vector<char>                   *content_;
	int                                 code_;
	int                                 subversion_;
	uint32                              max_num_;
};

static size_t ContentFunction(void *ptr, size_t size, size_t nmemb,
							  void *content){

    CurlContent* curl_content = (CurlContent*)content;
    
    if(curl_content->content_->size()<curl_content->max_num_){
        curl_content->content_->insert(curl_content->content_->end(),
             (char*)ptr,(char*)ptr+std::min(size*nmemb,
             curl_content->max_num_ - curl_content->content_->size()));	
    }
    return size*nmemb;
}

static size_t HeaderFunction(void* ptr,size_t size,
                             size_t nmemb,void* content){
    CurlContent * curl_content = (CurlContent*) content;
    char* start,*end;
    char* p = (char*)ptr;
    std::string name,value;
    if(!curl_content->code_){
        while(*p && isspace(*p))
            ++p;
        int32 rc = sscanf(p,"HTTP/1.%d %d",&curl_content->subversion_,
                           &curl_content->code_);
        if(rc!=2)
        	return -1;
         else 
         	return size* nmemb;	
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
	 
	 curl_content->headers_[name] = value;
	 return size* nmemb;
}

HttpPost::HttpPost(const MIG_URL& url)
:url_(url){

}


bool HttpPost::GetContent(std::string& content){
    for(std::vector<char>::iterator itr = content_.begin();
    	itr!=content_.end();++itr){
        content.append(1,(*itr));
    }
    return true;
}

bool HttpPost::Post(const char* post){
    CURL* curl = curl_easy_init();
    CURLcode curl_code;
    char curl_error[CURL_ERROR_SIZE];
    bool result = false;
    CurlContent curl_content;
    if(!curl){
        MIG_ERROR(USER_LEVEL,"curl_easy_init error");
        goto out;
    }
    
    curl_code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error);
    if(curl_code != CURLE_OK) {
        MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_ERRORBUFFER failed: %s",
                 curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_NOPROGRESS failed: %s",
			curl_easy_strerror(curl_code));
	goto out;
    }

	//设置超时
	curl_code = curl_easy_setopt(curl,CURLOPT_TIMEOUT,5);//5s超时设置
	if(curl_code!=CURLE_OK){
		MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_TIMEOUT failed: %s",
			curl_easy_strerror(curl_code));
		goto out;
	}

    curl_code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_NOSIGNAL failed: %s",
		   curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    if(curl_code != CURLE_OK) {
        MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_FAILONERROR failed: %s",
		  curl_easy_strerror(curl_code));
	goto out;
    }

#if defined(GOOGLE_URL)
    curl_code = curl_easy_setopt(curl,CURLOPT_URL,url_.spec().c_str());
#else
    curl_code = curl_easy_setopt(curl,CURLOPT_URL,url_.c_str());
#endif
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_URL failed: %s",
		curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
    if(curl_code != CURLE_OK){
        MIG_ERROR(USER_LEVEL,"curl_easy_setopt CURLOPT_POSTFILELDS failed %s",
            curl_easy_strerror(curl_code));
        goto out;
    }
	
    curl_content.code_ = 0;
    curl_content.content_ = &content_;
    curl_content.headers_ =  header_;
    curl_content.subversion_ = 0;
    curl_content.max_num_ = 1024*1024;
	
    /*curl_code = curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,HeaderFunction);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_HEADERFUNCTION failed: %s",
		  curl_easy_strerror(curl_code));
	goto out;
    }

   curl_code = curl_easy_setopt(curl,CURLOPT_HEADERDATA,&curl_content);
   if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_HEADERDATA failed: %s",
		  curl_easy_strerror(curl_code));
	goto out;
   }*/

   curl_code = curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,ContentFunction);
   if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_WRITEFUNCTION failed: %s",
		     curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl,CURLOPT_WRITEDATA,&curl_content);

    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_WRITEDATA failed: %s",
		  curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_perform(curl);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_perform failed: %s" ,
		      curl_easy_strerror(curl_code));
	goto out;
    }
    code_ = curl_content.code_;
    result = true;
out:
    curl_easy_cleanup(curl);
    return result;
}

HttpResponse::~HttpResponse() {

}

bool HttpResponse::GetContent(std::string& content){
    for(std::vector<char>::iterator itr = content_.begin();
    	itr!=content_.end();++itr){
        content.append(1, (*itr));
    }
    return true;
}


bool HttpResponse::Get(){
    CURL* curl = curl_easy_init();
    CURLcode curl_code;
    char curl_error[CURL_ERROR_SIZE];
    bool result = false;
    CurlContent curl_content;
    if(!curl){
        MIG_ERROR(USER_LEVEL,"curl_easy_init error");
        goto out;
    }
    
    curl_code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_ERRORBUFFER failed: %s",
		  curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_NOPROGRESS failed: %s",
		curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_NOSIGNAL failed: %s",
		  curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_FAILONERROR failed: %s",
		 curl_easy_strerror(curl_code));
	goto out;
    }


	//设置超时
	curl_code = curl_easy_setopt(curl,CURLOPT_TIMEOUT,5);//5s超时设置
	if(curl_code!=CURLE_OK){
		MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_TIMEOUT failed: %s",
			curl_easy_strerror(curl_code));
		goto out;
	}


#if defined(GOOGLE_URL)
    curl_code = curl_easy_setopt(curl,CURLOPT_URL,url_.spec().c_str());
#else
    curl_code = curl_easy_setopt(curl,CURLOPT_URL,url_.c_str());
#endif

    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_URL failed: %s",
			curl_easy_strerror(curl_code));
		goto out;
    }

    curl_content.code_ = 0;
    curl_content.content_ = &content_;
    curl_content.headers_ =  header_;
    curl_content.subversion_ = 0;
    curl_content.max_num_ = 1024*1024;
	
    curl_code = curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,HeaderFunction);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_HEADERFUNCTION failed: %s",
		     curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl,CURLOPT_HEADERDATA,&curl_content);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_HEADERDATA failed: %s",
		     curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,ContentFunction);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_WRITEFUNCTION failed: %s",
		     curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_setopt(curl,CURLOPT_WRITEDATA,&curl_content);

    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_setopt, CURLOPT_WRITEDATA failed: %s",
			 curl_easy_strerror(curl_code));
	goto out;
    }

    curl_code = curl_easy_perform(curl);
    if(curl_code != CURLE_OK) {
	MIG_ERROR(USER_LEVEL,"curl_easy_perform failed: %s" ,
		  curl_easy_strerror(curl_code));
	goto out;
    }
    code_ = curl_content.code_;
    result = true;
out:
    curl_easy_cleanup(curl);
    return result;
}

} // namespace mig_lbs
