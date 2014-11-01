#include "http_api.h"
#include "api_errno.h"
#include "basic/constants.h"


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




namespace base_net{

//注册接口
bool MYHttpApi::UserManager::OnUserRegister(packet::HttpPacket& packet,std::string& username,
		std::string& password,std::string& nickname,std::string& source,
		std::string& session,std::string& sex,std::string& borthday,std::string& location,
		std::string& head,int& err){
	bool r = false;
	std::string default_session = "1";
	std::string default_username = "default@miglab.com";
	std::string default_nickname = "米格用户";
	std::string default_sex = "1";
	std::string default_birthday= "1986-10=01";
	std::string default_location;//通过提交的或者坐标来判断城市
	std::string default_head;
	PARAM_IS_EXIST_RETURN("source",source,SOURCE_NOT_EXIST);
	//不为本平台即source 为0 则判断session
	if(source!="0"){
		PARAM_IS_EXIST_RETURN("session",session,SESSION_NOT_EXIST);
	}else{
		PARAM_IS_EXIST_DEFAULTPARAM("session",session,default_session);
	}

	//第三方平台用户名为空 密码为空 设置默认用户密码，具体到数据来分配
	if(source=="0"){
		PARAM_IS_EXIST_RETURN("username",username,USERNAME_NOT_EXIST);
		PARAM_IS_EXIST_RETURN("password",password,PASSWORD_NOT_EXIST);
		//本平台用户默认性别男
		PARAM_IS_EXIST_DEFAULTPARAM("sex",sex,default_sex);
		PARAM_IS_EXIST_DEFAULTPARAM("location",location,default_location);
		PARAM_IS_EXIST_DEFAULTPARAM("head",head,default_head);

	}else{
		PARAM_IS_EXIST_DEFAULTPARAM("username",username,default_username);
		PARAM_IS_EXIST_DEFAULTPARAM("password",password,default_username);
	}
	PARAM_IS_EXIST_DEFAULTPARAM("nickname",nickname,default_nickname);
	return true;
}

//登陆接口
bool MYHttpApi::UserManager::OnUserLogin(packet::HttpPacket& packet,std::string& username,
				std::string& password,std::string& clientid,int& err){

	bool r = false;
	PARAM_IS_EXIST_RETURN("username",username,USERNAME_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("password",password,PASSWORD_NOT_EXIST);

	return true;
}

//更新信息接口
bool MYHttpApi::UserManager::OnUserUpdate(packet::HttpPacket& packet,std::string& uid,
        std::string& nickname,std::string& gender,std::string& birthday,int& err){
	bool r = false;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("nickname",nickname,NICKNAME_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("gender",gender,SEX_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("birthday",birthday,BIRTHDAY_NOT_EXIST);
	return true;
}

/*
bool MYHttpApi::UserManager::OnBindPush(packet::HttpPacket& packet,std::string& uid,std::string& channel,std::string& userid,
		int& err){
	bool r = false;
	std::string token;
	PARAM_IS_EXIST_RETURN("uid",uid,UID_NOT_EXIST);
	PARAM_IS_EXIST_RETURN("token",token,TOKEN_EXIST);
	PARAM_IS_EXIST_RETURN("channelid",channel,CHANNLE_EXIST);
	PARAM_IS_EXIST_RETURN("userid",userid,CHANNLE_EXIST);
	return true;
}
*/

}

#undef PARAM_IS_EXIST_RETURN
#undef PARAM_IS_EXIST_DEFAULTPARAM
