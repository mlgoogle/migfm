#include "error_code.h"

const char* migchat_strerror(int error_code){

	switch(error_code){
		case MIG_CHAT_USER_NO_EXIST:
			return "用户不存在";
		default:
			return "未知";
	}

	return "未知";
}
