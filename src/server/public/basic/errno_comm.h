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


const char*
migfm_strerror(int error_code);
#endif