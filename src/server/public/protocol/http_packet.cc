#include "http_packet.h"

namespace packet{
	
HttpPacket::HttpPacket()
:type_("type"){
	data_ = new Data();
}

HttpPacket::HttpPacket(const char *data, const int32 len)
:type_("type"){
	data_ = new Data();
	SetData(data,len);
}

HttpPacket::HttpPacket(const HttpPacket& packet)
:type_("type")
,data_(packet.data_){
	if (data_!=NULL){
		data_->AddRef();
	}
}

HttpPacket& HttpPacket::operator =(const HttpPacket &packet){
	if (data_!=NULL)
		data_->Release();
	if (packet.data_!=NULL)
		packet.data_->AddRef();
	data_ =packet.data_;
	return *this;
}

void HttpPacket::PutAttrib(const std::string &strName, 
						   const std::string &strValue){
	data_->map_value_[strName] = strValue;
}

bool HttpPacket::GetAttrib(const std::string &strName, 
						   std::string &strValue){
    std::map<std::string,std::string>::iterator it =
		data_->map_value_.find(strName);
	if (it!=data_->map_value_.end()){
		strValue=(*it).second;
		return true;
	}
	return false;
}

void HttpPacket::HttpPacketDump(){
	for(std::map<std::string,std::string>::iterator it = data_->map_value_.begin();
		it!=data_->map_value_.end();++it){
			MIG_DEBUG(USER_LEVEL,"key[%s]:::value[%s]",it->first.c_str(),
				      it->second.c_str());
	}
}

bool HttpPacket::GetPacketType(std::string &strValue){
	return GetAttrib(type_,strValue);
}

void HttpPacket::SetData(const char* data,const int32 len){
	std::string request_str;
	request_str.assign(data,len);
	while (request_str.length()!=0){
		int32 start_pos = request_str.find("=");
		int32 end_pos = request_str.find("&")==-1?len:request_str.find("&");
		std::string key = request_str.substr(0,start_pos);
		std::string value = request_str.substr(key.length()+1,end_pos - key.length()-1);
		PutAttrib(key,value);
		if (request_str.find("&")!=-1)
			request_str = request_str.substr(end_pos+1,request_str.length());
		else
			request_str.clear();
	}
	return ;
}

}