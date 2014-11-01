#ifndef _RESTFUL_API_ERRNO_H__
#define _RESTFUL_API_ERRNO_H__




#define USERNAME_NOT_EXIST                  -1
#define PASSWORD_NOT_EXIST                  -2
#define NICKNAME_NOT_EXIST                  -3
#define SOURCE_NOT_EXIST                    -4
#define SESSION_NOT_EXIST                   -5
#define SEX_NOT_EXIST                       -6
#define BIRTHDAY_NOT_EXIST                  -7
#define LOCATION_NOT_EXIST                  -8
#define HEAD_NOT_EXIST                      -9
#define UID_NOT_EXIST                       -10
#define DB_ABNORMAL                         -101




#define USERNAME_NOT_EXIST_STR              "用户名不存在"
#define PASSWORD_NOT_EXIST_STR              "密码不存在"
#define NICKNAME_NOT_EXIST_STR              "昵称不存在"
#define SOURCE_NOT_EXIST_STR                "注册来源不存在"
#define SESSION_NOT_EXIST_STR               "第三方SESSION不存在"
#define SEX_NOT_EXIST_STR                   "性别不存在"
#define BIRTHDAY_NOT_EXIST_STR              "生日信息不存在"
#define LOCATION_NOT_EXIST_STR              "地区不存在"
#define HEAD_NOT_EXIST_STR                  "头像信息不存在"
#define UID_NOT_EXIST_STR                   "用户ID不存在"
#define UNKONW_ERROR                        "未知错误"

const char*
http_strerror(int error_code);

#endif
