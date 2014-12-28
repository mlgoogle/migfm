/*
 * error_comm.h
 *
 *  Created on: 2014年12月5日
 *      Author: kerry
 */

#ifndef ERROR_COMM_H_
#define ERROR_COMM_H_

enum errorcode{
	STRUCT_ERROR = -100,
	TYPE_LACK = -200,
	PLATFORM_LACK = -1000,
	UID_LACK = -1001,
	TOKEN_LACK = -1002,
	MUSIC_TYPE_LACK = -1003,
	MUSIC_TYPE_ID_LACK = -1004,
	MESSAGE_ID_LACK = -1005,
	GROUP_ID_LACK = -1006,
	TID_LACK = -1007
};


#define STRUCT_ERROR_STR                  "请求结构错误"
#define TYPE_LACK_STR                     "类别不存在"
#define PLATFORM_LACK_STR                 "平台号不存在"
#define UID_LACK_STR                      "缺少用户ID"
#define TID_LACK_STR                      "缺少对方ID"
#define TOKEN_LACK_STR                    "TOKEN不存在或错误"
#define MUSIC_TYPE_LACK_STR               "当前音乐类别不存在"
#define MUSIC_TYPE_ID_LACK_STR            "当前音乐类别ID不存在"
#define MESSAGE_ID_LACK_STR               "消息ID不存在"
#define GROUP_ID_LACK_STR                 "群组ID不存在"

const char*
miyo_strerror(int error_code);
#endif /* ERROR_COMM_H_ */
