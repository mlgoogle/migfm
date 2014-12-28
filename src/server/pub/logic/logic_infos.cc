/*
 * logic_infos.cc
 *
 *  Created on: 2014年12月2日
 *      Author: kerry
 */
#include "logic/logic_infos.h"
#include "logic_unit.h"

namespace base_logic{

LBSInfos::LBSInfos(){
	data_ = new Data();
}

LBSInfos::LBSInfos(const std::string& host,const double& latitude,
			const double& longitude,const std::string& city,const std::string& district,
			const std::string& province,std::string& street){
	data_ = new Data(host,latitude,longitude,city,district,province,street);
}

LBSInfos::LBSInfos(const LBSInfos& lbs_basic_info)
:data_(lbs_basic_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

LBSInfos& LBSInfos::operator =(const LBSInfos& lbs_basic_info){
	if(lbs_basic_info.data_!=NULL){
		lbs_basic_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = lbs_basic_info.data_;
	return (*this);
}


Dimension::Dimension(){
	data_ = new Data();
}

Dimension::Dimension(const int64 id,const std::string& name,const std::string& description){
	data_ = new Data(id,name,description);
}

Dimension::Dimension(const Dimension& dimension_info)
:data_(dimension_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

Dimension& Dimension::operator =(const Dimension& dimension_info){
	if(dimension_info.data_!=NULL){
		dimension_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = dimension_info.data_;
	return (*this);
}

Dimensions::Dimensions(){
	data_ = new Data();
}

Dimensions::Dimensions(const int64 id,const std::string& name,const std::string& alias){
	data_ = new Data(id,name,alias);
}

Dimensions::Dimensions(const Dimensions& dimensions_info)
:data_(dimensions_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

Dimensions& Dimensions::operator =(const Dimensions& dimensions_info){
	if(dimensions_info.data_!=NULL){
		dimensions_info.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = dimensions_info.data_;
	return (*this);
}

void Dimensions::dimension_name(const int64 id,std::string& name){
	std::string empty;
	std::map<int64,base_logic::Dimension>::iterator it = data_->dimensions_map_.find(id);
	if(it!=data_->dimensions_map_.end())
		name =  it->second.name();
	else
		name = UNKONW_DIMENSIONS;
}

}



