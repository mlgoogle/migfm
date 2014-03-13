#ifndef _MIG_CHAT_BASE_ERROR_CODE_H__
#define _MIG_CHAT_BASE_ERROR_CODE_H__



#define MIG_CHAT_USER_NO_EXIST          1
#define MIG_CHAT_USER_PASSWORD_ERROR    2


const char*
migchat_strerror(int error_code);
#endif
