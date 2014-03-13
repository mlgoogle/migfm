#include "user_connection.h"
#include "chat_cache_manager.h"
#include "logic_unit.h"
#include "base/logic_comm.h"
#include "base/error_code.h"

namespace chat_logic{

UserConnectionMgr::UserConnectionMgr(){

}

UserConnectionMgr::~UserConnectionMgr(){

}


bool UserConnectionMgr::OnUserLogin(struct server *srv, int socket, 
       struct PacketHead *packet, const void *msg /* = NULL*/ ,
	   int len /* = 0 */){
    
	struct UserLogin* usr_login = (struct UserLogin*)packet;
	chat_base::UserInfo userinfo;
	bool r = false;
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
	r = chat_logic::LogicUnit::CheckToken(usr_login->platform_id,
										usr_login->user_id,
										usr_login->token);

	if (!r){//password error
		//senderror(socket,USER_LOGIN_FAILED,0,usr_login->reserverd,MIG_CHAT_USER_PASSWORD_ERROR);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,usr_login->reserverd,MIG_CHAT_USER_PASSWORD_ERROR,__FILE__,__LINE__);
		return false;
	}
	r = chat_logic::LogicUnit::GetUserInfo(usr_login->platform_id,
			                               usr_login->user_id,userinfo);
	if (!r){//user vailed
		//senderror(socket,USER_LOGIN_FAILED,0,usr_logic->reserverd,MIG_CHAT_USER_NO_EXIST);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,usr_login->reserverd,MIG_CHAT_USER_NO_EXIST,__FILE__,__LINE__);
		return false;
	}

	userinfo.set_socket(socket);
	userinfo.set_platform_token(usr_login->token);

	r = pc->AddUserInfos(usr_login->platform_id,usr_login->user_id,userinfo);

	if (!r){// user vailed
		//senderror(socket,USER_LOGIN_FAILED,0,usr_login->reserverd,MIG_CHAT_USER_NO_EXIST);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,usr_login->reserverd,MIG_CHAT_USER_NO_EXIST,__FILE__,__LINE__);
		return false;
	}

	//返回新token 和session
	struct UserLoginSucess usr_loginsucess;
	MAKE_HEAD(usr_loginsucess, USER_LOGIN_SUCESS,USER_TYPE,0,usr_login->reserverd);
	usr_loginsucess.platform_id = usr_login->platform_id;
	usr_loginsucess.user_id = userinfo.user_id();
	usr_loginsucess.nick_number = userinfo.nicknumber();

	logic::SomeUtils::SafeStrncpy(usr_loginsucess.token,TOKEN_LEN,
			                      userinfo.chat_token().c_str(),
			                      userinfo.chat_token().length());

	logic::SomeUtils::SafeStrncpy(usr_loginsucess.nickname,NICKNAME_LEN,
			                      userinfo.nickname().c_str(),
			                      userinfo.nickname().length());

	logic::SomeUtils::SafeStrncpy(usr_loginsucess.head_url,HEAD_URL_LEN,
								  userinfo.head_url().c_str(),
								  userinfo.head_url().length());

	//return sendmessage(socket,&usr_loginsucess);
	return logic::SomeUtils::SendMessage(socket,&usr_loginsucess,__FILE__,__LINE__);
}


bool UserConnectionMgr::OnGetOppInfos(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg /*= NULL*/, int len/* = 0*/){

	struct ReqOppstionInfo* vReqOppstionInfo = (struct ReqOppstionInfo*)packet;
	bool r = false;
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();

	if(vReqOppstionInfo->type==1)//单用户
		return OnGetUserInfo(socket,vReqOppstionInfo->platform_id,vReqOppstionInfo->user_id,
							 vReqOppstionInfo->oppostion_id,vReqOppstionInfo->type,
							 vReqOppstionInfo->reserverd);
}

bool UserConnectionMgr::OnUserQuit(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg /*= NULL*/, int len/* = 0*/){

	struct UserQuit* vUserQuit = (struct UserQuit*)packet;
	bool r = false;
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
	r = chat_logic::LogicUnit::CheckToken(vUserQuit->platform_id,
			vUserQuit->user_id,vUserQuit->token);
	if (!r){//password error
		//senderror(socket,USER_LOGIN_FAILED,0,vUserQuit->reserverd,MIG_CHAT_USER_PASSWORD_ERROR);
		logic::SomeUtils::SendErrorCode(socket,USER_LOGIN_FAILED,ERROR_TYPE,0,vUserQuit->reserverd,MIG_CHAT_USER_PASSWORD_ERROR,__FILE__,__LINE__);
		return false;
	}


	//clear meeting session
	pc->DelMeetingInfos(vUserQuit->platform_id,vUserQuit->session,vUserQuit->user_id);
	pc->SendQuitInfoSession(vUserQuit->platform_id,vUserQuit->session,vUserQuit->user_id);


	return ClearUserinfo(vUserQuit->platform_id,vUserQuit->user_id);

}

bool UserConnectionMgr::OnGetUserInfo(const int socket,const int64 platform_id,const int64 user_id,
		const int64 oppinfo_id,const int32 type,const int64 usr_session){

	//get userinfo from cache
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
	chat_base::UserInfo userinfo;
	int64 session = 0 /*base::SysRadom::GetInstance()->GetRandomID()*/;
	bool r = pc->GetUserInfos(platform_id,user_id,userinfo);

	if(!r){ // offline

		//get userinfo from db
		if(!r)
			r = chat_logic::LogicUnit::GetUserInfo(platform_id,oppinfo_id,userinfo);
		if (!r){//user vailed
			senderror(socket,USER_LOGIN_FAILED,0,usr_session,MIG_CHAT_USER_NO_EXIST);
			return false;
		}

	}

	//select session
	r =pc->IsExitsLeaveInfos(platform_id,oppinfo_id,user_id,session);
	if(!r){//create leave message
		pc->AddLeaveInfos(platform_id,session,oppinfo_id,user_id);
		session = base::SysRadom::GetInstance()->GetRandomID();
	}



	struct OppositionInfo opposition_info;
	MAKE_HEAD(opposition_info, GET_OPPOSITION_INFO,USER_TYPE,0,usr_session);
	opposition_info.platform_id = platform_id;
	opposition_info.oppo_id = oppinfo_id;
	opposition_info.oppo_nick_number = userinfo.nicknumber();
	opposition_info.oppo_type = type;
	opposition_info.session = session;

	logic::SomeUtils::SafeStrncpy(opposition_info.oppo_nickname,NICKNAME_LEN,
								  userinfo.nickname().c_str(),userinfo.nickname().length());

    logic::SomeUtils::SafeStrncpy(opposition_info.oppo_user_head,HEAD_URL_LEN,
    							  userinfo.head_url().c_str(),userinfo.head_url().length());


    struct Oppinfo oppinfo;
    oppinfo.user_id = oppinfo_id;
    oppinfo.user_nicknumber = userinfo.nicknumber();
	logic::SomeUtils::SafeStrncpy(oppinfo.nickname,NICKNAME_LEN,
								  userinfo.nickname().c_str(),userinfo.nickname().length());

    logic::SomeUtils::SafeStrncpy(oppinfo.user_head,HEAD_URL_LEN,
    							  userinfo.head_url().c_str(),userinfo.head_url().length());

    opposition_info.opponfo_list.push_back(&oppinfo);


    //return sendmessage(socket,oppostion_info);
    return logic::SomeUtils::SendMessage(socket,&opposition_info,__FILE__,__LINE__);
}


bool UserConnectionMgr::ClearUserinfo(const int64 platform_id,const int64 user_id){
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();
	return pc->DelUserInos(platform_id,user_id);
}

}

