#include "error_code.h"

const char* migchat_strerror(int error_code){

	switch(error_code){
		case MIG_CHAT_USER_NO_EXIST:
			return "�û�������";
		default:
			return "δ֪";
	}

	return "δ֪";
}
