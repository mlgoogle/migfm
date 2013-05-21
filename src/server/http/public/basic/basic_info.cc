#include "basic_info.h"
#include <sstream>

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

MusicInfo::MusicInfo(){
	data_ = new Data();
}

MusicInfo::MusicInfo(const std::string id,const std::string& sid,const std::string& ssid,
					 const std::string& album_title,const std::string& title,
					 const std::string& url,const std::string& pub_time,
					 const std::string& artist){

		data_ = new Data(id,sid,ssid,album_title,title,url,pub_time,artist);
}


MusicInfo& MusicInfo::operator=(const MusicInfo& mi){
	if(mi.data_!=NULL){
		mi.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}

	data_ = mi.data_;
	return *this;
}

MusicInfo::MusicInfo(const MusicInfo& mi)
:data_(mi.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}
/*
{"sid":"1911362","ssid_":"9310","album_title":"VGhlIFVsdGltYXRlIEp1bmcuLi4=","titile_":"TGltYiBCeSBMaW1iIChESiBTUyBNaXgp","pub_time_":"2008","artist_":"Q3V0dHkgUmFua3M="}
*/
bool MusicInfo::SerializedJson(std::string &json){
	std::stringstream os;
	os<<"{\"id\":"
		<<"\""<<id().c_str()<<"\",\"sid\":"
		<<"\""<<sid().c_str()<<"\",\"ssid\":"
		<<"\""<<ssid().c_str()<<"\",\"album_title_\":"
		<<"\""<<album_title().c_str()<<"\",\"titile_\":"
		<<"\""<<title().c_str()<<"\",\"pub_time_\":"
		<<"\""<<pub_time().c_str()<<"\",\"artist_\":"
		<<"\""<<artist().c_str()<<"\"}";
	json.assign(os.str().c_str(),os.str().length());
	return true;

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
