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
	PLATFORM_LACK = -500,

	//登陆
	LOGIN_TYPE_LACK = -1100,
	IMEI_LACK = -1101,
	MACHINE_LACK = -1102,
	SOUCE_LACK = -1103,
	NICKNAME_LACK = -1104,
	SEX_LACK = -1105,
	SESSION_LACK = -1106,
	UID_LACK = -1107,
	TOKEN_LACK = -1108,
	BIRTHDAT_LACK = -1109,
	//推送绑定
	BD_CHANNEL_LACK = -1200,
	BD_USERID_LACK = -1201,
	PKG_NAME_LACK = -1202,
	TAG_LACK = -1203,
	APPID_LACK = -1204,
	REQUEST_LACK = -1205,


	//音乐相关
	MUSIC_TYPE_LACK = -2000,
	MUSIC_TYPE_ID_LACK = -2001,
	MUSIC_SONG_ID_LACK = -2002,

	//社交相关
	MESSAGE_ID_LACK = -3000,
	GROUP_ID_LACK = -3001,
	TID_LACK = -3002,
	MSG_CONTENT_LACK = -3003
};


#define STRUCT_ERROR_STR                  "请求结构错误"
#define TYPE_LACK_STR                     "类别不存在"
#define PLATFORM_LACK_STR                 "平台号不存在"
#define UID_LACK_STR                      "缺少用户ID"
#define TID_LACK_STR                      "缺少对方ID"
#define BIRTHDAT_LACK_STR                 "缺少生日"
#define TOKEN_LACK_STR                    "TOKEN不存在或错误"
#define MUSIC_TYPE_LACK_STR               "当前音乐类别不存在"
#define MUSIC_TYPE_ID_LACK_STR            "当前音乐类别ID不存在"
#define MESSAGE_ID_LACK_STR               "消息ID不存在"
#define GROUP_ID_LACK_STR                 "群组ID不存在"
#define MSG_CONTENT_LACK_STR              "消息内容不存在"
#define MUSIC_SONG_ID_LACK_STR            "音乐ID不存在"

const char*
miyo_strerror(int error_code);
#endif /* ERROR_COMM_H_ */
