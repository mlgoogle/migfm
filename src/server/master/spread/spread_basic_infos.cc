#include "spread_basic_infos.h"

namespace base{

WeiXin::GraphicTextInfo::GraphicTextInfo(){
	data_ = new Data();
}

WeiXin::GraphicTextInfo::GraphicTextInfo(const std::string& title,const std::string& desction,
				const std::string& picurl,const std::string& url){
	data_ = new Data(title,desction,picurl,url);
}


WeiXin::GraphicTextInfo::GraphicTextInfo(const GraphicTextInfo& graphic_info)
:data_(graphic_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

WeiXin::GraphicTextInfo& WeiXin::GraphicTextInfo::operator =(const GraphicTextInfo& graphic_info){

	if (graphic_info.data_!=NULL){
		graphic_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = graphic_info.data_;
	return *this;
}



}
