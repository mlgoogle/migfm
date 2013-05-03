#include "basic_info.h"

namespace base{
	
MusicUsrInfo::MusicUsrInfo(){
    data_ = new Data();
}

MusicUsrInfo::MusicUsrInfo(const std::string& name,const std::string& phone,
                           const std::string& content){
    data_ = new Data(name,phone,content);
}


MusicUsrInfo& MusicUsrInfo::operator=(const MusicUsrInfo& mi){
    if(mi.data_!=NULL){
        mi.data_->AddRef();
    }
    
    if(data_!=NULL){
        data_->Release();
    }

    data_ = mi.data_;
    return *this;
}

MusicUsrInfo::MusicUsrInfo(const MusicUsrInfo& mi)
:data_(mi.data_){
    if(data_!=NULL){
        data_->AddRef();
    }
}

ConnAddr::ConnAddr(){
  
    data_ = new Data();
}

ConnAddr::ConnAddr(const std::string& host,const int port,
					const std::string& usr,const std::string& pwd,
					const std::string& source){
    
    data_ = new Data(host,port,usr,pwd,source);
}

ConnAddr& ConnAddr::operator=(const ConnAddr& ca){
	if(ca.data_!=NULL){
		ca.data_->AddRef();
    }
    if(data_!=NULL){
    	data_->Release();
    }

    data_ = ca.data_;
       return *this;
}

ConnAddr::ConnAddr(const ConnAddr& ca)
:data_(ca.data_){
	if(data_!=NULL){
	    data_->AddRef();
	}
}

}
