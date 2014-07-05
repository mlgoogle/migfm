#include "robot_basic_info.h"
#include "basic/constants.h"
#include "json/json.h"
#include "log/mig_log.h"

namespace robot_base{


SchedulerInfo::SchedulerInfo(){
	data_ = new Data();
}

SchedulerInfo::SchedulerInfo(const int64 platform_id,const int socket,const std::string& ip,
		const std::string& machine_id){
	data_ = new Data(platform_id,socket,ip,machine_id);
}

SchedulerInfo::SchedulerInfo(const SchedulerInfo& scheduler_info)
:data_(scheduler_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

SchedulerInfo& SchedulerInfo::operator =(const SchedulerInfo& scheduler_info){

	if (scheduler_info.data_!=NULL){
		scheduler_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = scheduler_info.data_;
	return *this;
}


RobotBasicInfo::RobotBasicInfo(){
	data_ = new Data();
}

RobotBasicInfo::RobotBasicInfo(const int64 uid,const int32 sex,const double latitude,
			                  const double longitude,const int64 songid,const std::string nickname,
			                  const std::string& head_url){
	data_ = new Data(uid,sex,latitude,longitude,songid,nickname,head_url);
}

RobotBasicInfo::RobotBasicInfo(const RobotBasicInfo& robot_info)
:data_(robot_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

RobotBasicInfo& RobotBasicInfo::operator =(const RobotBasicInfo& robot_info){

	if (robot_info.data_!=NULL){
		robot_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = robot_info.data_;
	return *this;
}


PlatformInfo::PlatformInfo(){
    data_ = new Data();
}

PlatformInfo::PlatformInfo(const int64 platform_id,
						   const std::string& platform_name){
	data_ = new Data(platform_id,platform_name);
}

PlatformInfo::PlatformInfo(const PlatformInfo& platform_info)
:data_(platform_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

PlatformInfo& PlatformInfo::operator =(const PlatformInfo& platfrom_info){

	if (platfrom_info.data_!=NULL){
		platfrom_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = platfrom_info.data_;
	return *this;
}

UserBasicInfo::UserBasicInfo(){
	data_ = new Data();
}

UserBasicInfo::UserBasicInfo(const int64 uid){
	data_ = new Data(uid);
}

UserBasicInfo& UserBasicInfo::operator =(const UserBasicInfo& user_info){
	if (user_info.data_!=NULL){
		user_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = user_info.data_;
	return *this;
}


UserBasicInfo::UserBasicInfo(const UserBasicInfo& user_info)
:data_(user_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}


UserLbsInfo::UserLbsInfo(){
    data_ = new Data();
}

UserLbsInfo::UserLbsInfo(const int64 uid,const std::string& latitude,const std::string& longitude){
	data_ = new Data(uid,latitude,longitude);
}

UserLbsInfo::UserLbsInfo(const UserLbsInfo& userlbs_info)
:data_(userlbs_info.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

UserLbsInfo& UserLbsInfo::operator =(const UserLbsInfo& userlbs_info){

	if (userlbs_info.data_!=NULL){
		userlbs_info.data_->AddRef();
	}
	if (data_!=NULL){
		data_->Release();
	}
	data_ = userlbs_info.data_;
	return *this;
}


}
