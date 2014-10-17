#ifndef MIG_FM_PUBLIC_BASIC_ERRNO_COMM__H__
#define MIG_FM_PUBLIC_BASIC_ERRNO_COMM__H__

#define  MIG_FM_HTTP_MOOD_NO_VALID               -1
#define  MIG_FM_HTTP_USER_NO_EXITS               -2
#define  MIG_FM_USER_MOOD_NO_EXITS               -3
#define  MIG_FM_HTTP_MODE_NO_VALID               -4
#define  MIG_FM_HTTP_MOOD_DEC_NO_EXITS           -5
#define  MIG_FM_HTTP_DEC_NO_VALID                -6
#define  MIG_FM_DEC_NO_VALID                     -7
#define  MIG_FM_HTTP_CHANNLE_NO_VALID            -8
#define  MIG_FM_HTTP_SONG_ID_NO_VALID            -9
#define  MIG_FM_USER_NO_COLLECT_SONG             -10
#define  MIG_FM_MOOD_NO_VALID                    -11
#define  MIG_FM_SYSTEM_DEC_NO_VALID              -12
#define  MIG_FM_MOODID_NO_VALID                  -13
#define  MIG_FM_MOODINDEX_NO_VALID               -14
#define  MIG_FM_SCENEID_NO_VALID                 -15
#define  MIG_FM_SCENEINDEX_NO_VALID              -16
#define  MIG_FM_CHANNELID_NO_VALID               -17
#define  MIG_FM_CHANNELINDEX_NO_VALID            -18
#define  MIG_FM_HTTP_LAST_SONG_ID_NO_VALID       -19
#define  MIG_FM_SOUCE_NO_VALID                   -20
#define  MIG_FM_CONTENT_NO_EXITS                 -21
#define  MIG_FM_VERSION_NO_EXITS                 -22
#define  MIG_FM_NET_STATE_NO_EXITS               -23
#define  MIG_FM_HTTP_INVALID_USER_ID           -24		// "请求中ID字段非法";
#define  MIG_FM_HTTP_DEVICE_TOKEN_NOT_EXIST   	-25		// "请求中未包含devicetoken字段";
#define  MIG_FM_HTTP_INVALID_TIME_FORMAT    	-26		// "请求中时间格式非法";
#define  MIG_FM_DB_SAVE_PUSH_CONFIG_FAILED 		-27		// "数据库保存推送设置失败";
#define  MIG_FM_DB_READ_PUSH_CONFIG_FAILED 		-28		// "数据库读取推送设置失败";
#define  MIG_FM_OTHER_PUSH_SERVICE_CLOSED 		-29		// "对方推送服务关闭"
#define  MIG_FM_OTHER_ANTI_HARASSMENT 			-30		// "对方开启防打扰"
#define  MIG_FM_DB_ACCESS_FAILED    			-31		// "数据库访问失败"
#define  MIG_FM_PUSH_MSG_FAILED   				-32		// "推送消息失败"
#define  MIG_FM_MSG_LIST_EMPTY     				-33 	// "消息列表为空"
#define  MIG_FM_SOURCE_NO_VAILED                -34  //"请求中未包含来源";
#define  MIG_FM_SOURCE_SESSION_NO_VAILED        -35  //"请求中未包含第三方标示符";
#define  MIG_FM_USERNAME_NO_VAILED              -36  //"请求中未包含用户名";
#define  MIG_FM_PASSWORD_NO_VAILED              -37  //"请求中未包含密码";
#define  MIG_FM_USER_EXITS                      -38   //用户存在
#define  MIG_FM_HTTP_COMMENT_INVALID			-39		// "请求中评论字段无效"
#define  MIG_FM_HTTP_FROMID_INVALID				-40		// "请求中起始ID无效"
#define  MIG_FM_HTTP_COUNT_INVALID				-41		// "请求中分页数量无效"
#define  MIG_FM_HTTP_PLAT_INVALID				-42		// "登录平台信息错误"
#define  MIG_FM_HTTP_USERCHECK_INVALID				-43		// "用户名，密码错误"
#define  MIG_FM_HTTP_USERINFO_INVALID				-44		// "无效用户信息"
#define  MIG_FM_HTTP_JSON_ERROR                  -45 //"JSON格式错误"
#define  MIG_FM_SHARE_TYPE                       -46 //分享类别不存在
#define  MIG_FM_NO_LYRIC                         -47// 没有歌词
#define  MIG_FM_NO_LOCATION                      -48// 没有提供坐标
#define  MIG_FM_NO_TOKEN                         -49// 不含token
#define  MIG_FM_TOKEN_ERRNO                      -50 //token验证失败或者失效
const char*
migfm_strerror(int error_code);
#endif
