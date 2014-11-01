#include "api_errno.h"

const char*
http_strerror(int error_code){
	switch (error_code){
	case USERNAME_NOT_EXIST:
		return USERNAME_NOT_EXIST_STR;
	case PASSWORD_NOT_EXIST:
		return PASSWORD_NOT_EXIST_STR;
	case NICKNAME_NOT_EXIST:
		return NICKNAME_NOT_EXIST_STR;
	case SOURCE_NOT_EXIST:
		return SOURCE_NOT_EXIST_STR;
	case SESSION_NOT_EXIST:
		return SESSION_NOT_EXIST_STR;
	case SEX_NOT_EXIST:
		return SEX_NOT_EXIST_STR;
	case BIRTHDAY_NOT_EXIST:
		return BIRTHDAY_NOT_EXIST_STR;
	case LOCATION_NOT_EXIST:
		return LOCATION_NOT_EXIST_STR;
	case HEAD_NOT_EXIST:
		return HEAD_NOT_EXIST_STR;
	case UID_NOT_EXIST:
		return UID_NOT_EXIST_STR;
	case TID_NOT_EXIST:
		return TID_NOT_EXIST_STR;
	case MSGID_NOT_EXIST:
		return MSGID_NOT_EXIST_STR;
	default:
		return UNKONW_ERROR;
	}
}
