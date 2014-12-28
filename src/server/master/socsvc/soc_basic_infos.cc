/*
 * soc_basic_infos.cc
 *
 *  Created on: 2014年12月10日
 *      Author: kerry
 */
#include "soc_basic_infos.h"
namespace socsvc_logic{

BarrageInfos::BarrageInfos(){
	data_ = new Data();
}

BarrageInfos::BarrageInfos(const int64 msgid,const int64 fid,const std::string& nickname,
			const std::string& message){
	data_ = new Data(msgid,fid,nickname,message);
}

BarrageInfos::BarrageInfos(const BarrageInfos& barrageinfo)
:data_(barrageinfo.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

BarrageInfos& BarrageInfos::operator =(const BarrageInfos& barrageinfo){
	if(barrageinfo.data_!=NULL){
		barrageinfo.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = barrageinfo.data_;
	return (*this);
}

}




