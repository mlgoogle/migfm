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
#define  MIG_FM_HTTP_INVALID_USER_ID           -24		// "������ID�ֶηǷ�";
#define  MIG_FM_HTTP_DEVICE_TOKEN_NOT_EXIST   	-25		// "������δ����devicetoken�ֶ�";
#define  MIG_FM_HTTP_INVALID_TIME_FORMAT    	-26		// "������ʱ���ʽ�Ƿ�";
#define  MIG_FM_DB_SAVE_PUSH_CONFIG_FAILED 		-27		// "���ݿⱣ����������ʧ��";
#define  MIG_FM_DB_READ_PUSH_CONFIG_FAILED 		-28		// "���ݿ��ȡ��������ʧ��";
#define  MIG_FM_OTHER_PUSH_SERVICE_CLOSED 		-29		// "�Է����ͷ���ر�"
#define  MIG_FM_OTHER_ANTI_HARASSMENT 			-30		// "�Է�����������"
#define  MIG_FM_DB_ACCESS_FAILED    			-31		// "���ݿ����ʧ��"
#define  MIG_FM_PUSH_MSG_FAILED   				-32		// "������Ϣʧ��"
#define  MIG_FM_MSG_LIST_EMPTY     				-33 	// "��Ϣ�б�Ϊ��"
#define  MIG_FM_SOURCE_NO_VAILED                -34  //"������δ������Դ";
#define  MIG_FM_SOURCE_SESSION_NO_VAILED        -35  //"������δ������������ʾ��";
#define  MIG_FM_USERNAME_NO_VAILED              -36  //"������δ�����û���";
#define  MIG_FM_PASSWORD_NO_VAILED              -37  //"������δ��������";
#define  MIG_FM_USER_EXITS                      -38   //�û�����
#define  MIG_FM_HTTP_COMMENT_INVALID			-39		// "�����������ֶ���Ч"
#define  MIG_FM_HTTP_FROMID_INVALID				-40		// "��������ʼID��Ч"
#define  MIG_FM_HTTP_COUNT_INVALID				-41		// "�����з�ҳ������Ч"
#define  MIG_FM_HTTP_PLAT_INVALID				-42		// "��¼ƽ̨��Ϣ����"
#define  MIG_FM_HTTP_USERCHECK_INVALID				-43		// "�û������������"
#define  MIG_FM_HTTP_USERINFO_INVALID				-44		// "��Ч�û���Ϣ"
#define  MIG_FM_HTTP_JSON_ERROR                  -45 //"JSON��ʽ����"
const char*
migfm_strerror(int error_code);
#endif
