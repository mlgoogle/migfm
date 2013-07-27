/*
*
* Returns: a static string.
*/
#include "errno_comm.h"
const char*
migfm_strerror(int error_code){
	
	switch (error_code){
		case MIG_FM_HTTP_MOOD_NO_VALID:
			return "请求中未包含心情信息";
		case MIG_FM_HTTP_USER_NO_EXITS:
			return "请求中未包含用户信息";
		case MIG_FM_USER_MOOD_NO_EXITS:
			return "用户心情未生成";
		case MIG_FM_HTTP_MODE_NO_VALID:
			return "请求中未包含模式信息";
		case MIG_FM_HTTP_MOOD_DEC_NO_EXITS:
			return "请求中未包含心情描述信息";
		case MIG_FM_HTTP_DEC_NO_VALID:
			return "请求中未包含模式描述信息";
		case MIG_FM_HTTP_CHANNLE_NO_VALID:
			return "请求中未包含频道信息";
		case MIG_FM_HTTP_SONG_ID_NO_VALID:
			return "请求中未包含歌曲信息";
		case MIG_FM_USER_NO_COLLECT_SONG:
			return "用户未收藏歌曲";
		case MIG_FM_MOOD_NO_VALID:
			return "系统中心情不存在";
		case MIG_FM_SYSTEM_DEC_NO_VALID:
			return "系统中描述词不存在";
		default:
			return "未知错误码";

	}
}