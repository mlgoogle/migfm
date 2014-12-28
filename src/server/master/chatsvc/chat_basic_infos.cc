/*
 * chat_basic_infos.cc
 *
 *  Created on: 2014年12月10日
 *      Author: kerry
 */
#include "chat_basic_infos.h"
namespace chatsvc_logic{

AloneMessageInfos::AloneMessageInfos(){
	data_ = new Data();
}

AloneMessageInfos::AloneMessageInfos(const int64 platform,const int64 fid,const int64 tid,
		const int64 msg_id,const std::string& nickname,
		const std::string& message,const std::string& lasttime,const std::string& pic){

	data_ = new Data(platform,msg_id,fid,tid,nickname,message,lasttime,pic);
}

AloneMessageInfos::AloneMessageInfos(const AloneMessageInfos& alonemsginfo)
:data_(alonemsginfo.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

AloneMessageInfos& AloneMessageInfos::operator = (const AloneMessageInfos& alonemsginfo){
	if(alonemsginfo.data_!=NULL){
		alonemsginfo.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = alonemsginfo.data_;
	return (*this);
}



GroupMessageInfos::GroupMessageInfos(){
	data_ = new Data();
}

GroupMessageInfos::GroupMessageInfos(const int64 platform,const int64 groupid,const int64 msg_id,const int64 fid,const std::string& nickname,
		const std::string& message,const std::string& lasttime){

	data_ = new Data(platform,groupid,msg_id,fid,nickname,message,lasttime);
}

GroupMessageInfos::GroupMessageInfos(const GroupMessageInfos& groupmsginfo)
:data_(groupmsginfo.data_){
	if(data_!=NULL){
		data_->AddRef();
	}
}

GroupMessageInfos& GroupMessageInfos::operator = (const GroupMessageInfos& groupmsginfo){
	if(groupmsginfo.data_!=NULL){
		groupmsginfo.data_->AddRef();
	}

	if(data_!=NULL){
		data_->Release();
	}
	data_ = groupmsginfo.data_;
	return (*this);
}

}




