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
#define TID_NOT_EXIST                       -11
#define MSGID_NOT_EXIST                     -12
#define CHANNEL_ID_NOT_EXIST                -13
#define MODE_NOT_EXIST                      -14
#define MODE_ID_NOT_EXIST                   -15
#define SONG_ID_NOT_EXIST                   -16
#define VERSION_NOT_EXIST                   -17
#define TYPE_INDEX_NOT_EXIST                -18
#define MOOD_INDEX_NOT_EXIST                -19
#define SCENE_INDEX_NOT_EXIST               -20
#define MOOD_ID_NOT_EXIST                   -21
#define SCENCE_ID_NOT_EXIST                 -22
#define TAR_UID_NOT_EXIST                   -23
#define CUR_SONG_NOT_EXIST                  -24
#define LAST_SONG_NOT_EXIST                 -25
#define NAME_SONG_NOT_EXIST                 -26
#define SINGER_NOT_EXIST                    -27
#define STATE_NOT_EXIST                     -28
#define SONG_ID_NOT_EXIST                   -29
#define PLAT_ID_NOT_EXIST                   -30
#define LOCATION_NOT_EXIST                  -31
#define EXT_MSG_NOT_EXIST                   -32
#define COMMENT_MSG_NOT_EXIST               -33
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
#define TID_NOT_EXIST_STR                   "对方ID不存在"
#define MSGID_NOT_EXIST_STR                 "消息ID不存在"
#define CHANNEL_ID_NOT_EXIST_STR            "频道号不存在"
#define MODE_NOT_EXIST_STR                  "类别不存在"
#define MODE_ID_NOT_EXIST_STR               "类别ID不存在"
#define MOOD_ID_NOT_EXIST_STR               "心情ID不存在"
#define SENCE_ID_NOT_STR                    "场景ID不存在"
#define SONG_ID_NOT_EXIST_STR               "歌曲ID不存在"
#define VERSION_NOT_EXIST_STR               "版本号不存在"
#define TYPE_INDEX_NOT_EXIST_STR            "类别权重不存在"
#define MOOD_INDEX_NOT_EXIST_STR            "心情权重不存在"
#define SCENE_INDEX_NOT_EXIST_STR           "场景权重不存在"
#define TAR_UID_NOT_EXIST_STR               "对方ID不存在"
#define CUR_SONG_NOT_EXIST_STR              "当前歌曲不存在"
#define LAST_SONG_NOT_EXIST_STR             "前首歌不存在"
#define NAME_SONG_NOT_EXIST_STR             "歌曲名不存在"
#define SINGER_NOT_EXIST_STR                "歌手不存在"
#define STATE_NOT_EXIST_STR                 "缺少网络状态"
#define SONG_ID_NOT_EXIST_STR               "歌曲ID不存在"
#define PLAT_ID_NOT_EXIST_STR               "分享平台ID不存在"
#define LOCATION_NOT_EXIST_STR              "缺少坐标"
#define EXT_MSG_NOT_EXIST_STR               "缺少附加信息"
#define COMMENT_MSG_NOT_EXIST_STR           "缺少评价内容"
#define UNKONW_ERROR                        "未知错误"

const char*
http_strerror(int error_code);

#endif
