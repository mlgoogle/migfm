#ifndef _MIG_FM_BASE_PUSH_LOGIC_UNIT_H__
#define _MIG_FM_BASE_PUSH_LOGIC_UNIT_H__
#include "basic/basictypes.h"
#include "protocol/http_packet.h"
#include "json/json.h"
#include <string>

#define  PUSH_PLATFORM_NOT_EXIST           -1
#define  PUSH_UID_NOT_EXIST                -2
#define  PUSH_CHANNEL_NOT_EXIST            -3
#define  PUSH_USERID_NOT_EXIST             -4
#define  PUSH_PKG_NOT_EXIST                -5
#define  PUSH_MACHINE_NOT_EXIST            -6
#define  PUSH_TAG_NOT_EXIST                -7
#define  PUSH_APP_ID_NOT_EXIST             -8
#define  PUSH_REQUEST_ID_NOT_EXIST         -9
#define  PUSH_DB_STORAGE                   -10

#define  PUSH_PLATFORM_NOT_EXIST_STR             "平台号不存在"
#define  PUSH_UID_NOT_EXIST_STR                  "用户ID不存在"
#define  PUSH_CHANNEL_NOT_EXIST_STR              "频道号不存在"
#define  PUSH_USERID_NOT_EXIST_STR               "百度UID不存在"
#define  PUSH_PKG_NOT_EXIST_STR                  "包名不存在"
#define  PUSH_MACHINE_NOT_EXIST_STR              "机型不存在"
#define  PUSH_TAG_NOT_EXIST_STR                  "tag不存在"
#define  PUSH_APP_ID_NOT_EXIST_STR               "app id不存在"
#define  PUSH_REQUEST_ID_NOT_EXIST_STR            "request id 不存在"
#define  PUSH_DB_STORAGE_STR                     "写入数据库失败"
#define  PUSH_UNKONW_ERROR                       "未知错误"

namespace base_push{

const char*
push_http_strerror(int error_code);

class ResolveJson{
public:
	static bool ReolveJsonBaiduPushMsg(const std::string& content,int64& request_id,
			int32& success_amount,std::string& resouce_ids,int32& eror_code,
			std::string& err_msg);
};

class PushNet{
public:
	static bool OnBDBindPush(packet::HttpPacket& packet,std::string& platform,
			std::string& uid,std::string& channel,std::string& userid,std::string& pkg_name,
			std::string& tag,std::string& machine,std::string& appid,std::string& requestid,int& err);
};

}
#endif
