#include "push_logic_unit.h"

#define PARAM_IS_EXIST_RETURN(NAME,PARAM,ERR_CODE) \
	r = packet.GetAttrib(NAME,PARAM); \
	if(!r||PARAM.empty()){\
		err = ERR_CODE;\
		return false;\
	}\


#define PARAM_IS_EXIST_DEFAULTPARAM(NAME,PARAM,DEFAULT)\
	r = packet.GetAttrib(NAME,PARAM); \
	if(!r||PARAM.empty()){\
		if(!DEFAULT.empty())\
			PARAM = DEFAULT;\
	}\


namespace base_push{

const char*
push_http_strerror(int error_code){
	switch (error_code){
	case PUSH_PLATFORM_NOT_EXIST:
		return PUSH_PLATFORM_NOT_EXIST_STR;
	case PUSH_UID_NOT_EXIST:
		return PUSH_UID_NOT_EXIST_STR;
	case PUSH_CHANNEL_NOT_EXIST:
		return PUSH_CHANNEL_NOT_EXIST_STR;
	case PUSH_USERID_NOT_EXIST:
		return PUSH_USERID_NOT_EXIST_STR;
	case PUSH_PKG_NOT_EXIST:
		return PUSH_PKG_NOT_EXIST_STR;
	case PUSH_MACHINE_NOT_EXIST:
		return PUSH_MACHINE_NOT_EXIST_STR;
	case PUSH_DB_STORAGE:
		return PUSH_DB_STORAGE_STR;
	case PUSH_APP_ID_NOT_EXIST:
		return PUSH_APP_ID_NOT_EXIST_STR;
	case PUSH_REQUEST_ID_NOT_EXIST:
		return PUSH_REQUEST_ID_NOT_EXIST_STR;
	default:
		return PUSH_UNKONW_ERROR;
	}
}



bool ResolveJson::ReolveJsonBaiduPushMsg(const std::string& content,int64& request_id,
			int32& success_amount,std::string& resouce_ids,int32& eror_code,
			std::string& err_msg){
	bool r = false;
	Json::Reader reader;
	Json::Value  root;
	Json::Value reponse_params;
	r = reader.parse(content.c_str(),root);
	if(!r)
		return r;
	if (!root.isMember("request_id"))
		return false;
	request_id = root["request_id"].asInt64();

	//是否存在错误error_code

	if (!root.isMember("error_code")){//正确
		//
		if(!root.isMember("response_params"))
			return false;
		reponse_params = root["response_params"];

		if(!reponse_params.isMember("success_amount"))
			return false;
		success_amount = reponse_params["success_amount"].asInt();

		/*if(!reponse_params.isMember("resource_ids"))
			return false;
		resouce_ids = reponse_params["resource_ids"].asString();*/
		return true;
	}else{//错误
		eror_code = root["error_code"].asInt();
		if(!root.isMember("error_msg"))
			return false;
		err_msg = root["error_msg"].asString();
		return  false;
	}
}


bool PushNet::OnBDBindPush(packet::HttpPacket& packet,std::string& platform,
		std::string& uid,std::string& channel,std::string& userid,std::string& pkg_name,
		std::string& tag,std::string& machine,std::string& appid,std::string& requestid,
		int& err){
	bool r = false;
	std::string token;
	PARAM_IS_EXIST_RETURN("platform",platform,PUSH_PLATFORM_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("uid",uid,PUSH_UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("channel",channel,PUSH_CHANNEL_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("userid",userid,PUSH_USERID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("pkg_name",pkg_name,PUSH_PKG_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("tag",tag,PUSH_TAG_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("machine",machine,PUSH_MACHINE_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("appid",appid,PUSH_APP_ID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("requestid",requestid,PUSH_REQUEST_ID_NOT_EXIST);
	return true;
}


}

#undef PARAM_IS_EXIST_RETURN
#undef PARAM_IS_EXIST_DEFAULTPARAM
