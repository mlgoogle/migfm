#ifndef _MIG_FM_PUBLIC_BASIC_BASIC_INFO_H__
#define _MIG_FM_PUBLIC_BASIC_BASIC_INFO_H__
#include <stdio.h>
#include "constants.h"
namespace base{

class ConnAddr{
public:
    explicit ConnAddr();
    explicit ConnAddr(const std::string& host,const int port,
    				const std::string& usr="",const std::string& pwd="",const std::string& source="");

    ConnAddr(const ConnAddr& ca);
	ConnAddr& operator=(const ConnAddr& ca);
	
    ~ConnAddr(){
        if(data_!=NULL){
            data_->Release();
        }
    } 
    
    const std::string& host() const {return !data_?STR_EMPTY:data_->host_;}
    const int port() const {return !data_?0:data_->port_;}
    const std::string& usr() {return !data_?STR_EMPTY:data_->usr_;}
    const std::string& pwd() {return !data_?STR_EMPTY:data_->pwd_;}   
    const std::string& source() {return !data_?STR_EMPTY:data_->source_;} 	

private:
   class Data{
    public:
	Data():refcount_(1),port_(0){}
	Data(const std::string& host,const int port,const std::string& usr,const std::string& pwd,const std::string& source)
	   :host_(host)
	   ,port_(port)
	   ,usr_(usr)
	   ,pwd_(pwd)
	   ,source_(source)
       ,refcount_(1){}
    void AddRef(){refcount_++;}
    void Release() {if(!--refcount_) delete this;}
    const std::string host_;
    const int port_;
    const std::string usr_;
    const std::string pwd_;
    const std::string source_;
    private:
	int refcount_;
    };
    
    Data*    data_;
};

}

#endif
