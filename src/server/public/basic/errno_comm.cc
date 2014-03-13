/*
*
* Returns: a static string.
*/
#include "errno_comm.h"
const char*
migfm_strerror(int error_code){
	
	switch (error_code) {
	case MIG_FM_HTTP_MOOD_NO_VALID:
		return "������δ����������Ϣ";
	case MIG_FM_HTTP_USER_NO_EXITS:
		return "������δ�����û���Ϣ";
	case MIG_FM_USER_MOOD_NO_EXITS:
		return "�û�����δ����";
	case MIG_FM_HTTP_MODE_NO_VALID:
		return "������δ����ģʽ��Ϣ";
	case MIG_FM_HTTP_MOOD_DEC_NO_EXITS:
		return "������δ��������������Ϣ";
	case MIG_FM_HTTP_DEC_NO_VALID:
		return "������δ����ģʽ������Ϣ";
	case MIG_FM_HTTP_CHANNLE_NO_VALID:
		return "������δ����Ƶ����Ϣ";
	case MIG_FM_HTTP_SONG_ID_NO_VALID:
		return "������δ����������Ϣ";
	case MIG_FM_USER_NO_COLLECT_SONG:
		return "�û�δ�ղظ���";
	case MIG_FM_MOOD_NO_VALID:
		return "ϵͳ�����鲻����";
	case MIG_FM_SYSTEM_DEC_NO_VALID:
		return "ϵͳ�������ʲ�����";
	case MIG_FM_MOODID_NO_VALID:
		return "������δ�����������";
	case MIG_FM_MOODINDEX_NO_VALID:
		return "������δ��������˳��";
	case MIG_FM_SCENEID_NO_VALID:
		return "������δ�����������";
	case MIG_FM_SCENEINDEX_NO_VALID:
		return "������δ��������˳��";
	case MIG_FM_CHANNELID_NO_VALID:
		return "������δ����Ƶ�����";
	case MIG_FM_CHANNELINDEX_NO_VALID:
		return "������δ����Ƶ��˳��";
	case MIG_FM_SOUCE_NO_VALID:
		return "������δ������Դ";
	case MIG_FM_CONTENT_NO_EXITS:
		return "������δ����������Ϣ";
	case MIG_FM_VERSION_NO_EXITS:
		return "������δ�����汾��Ϣ";
	case MIG_FM_NET_STATE_NO_EXITS:
		return "������δ��������״̬";
	case MIG_FM_HTTP_INVALID_USER_ID:
		return "������ID�ֶηǷ�";
	case MIG_FM_HTTP_DEVICE_TOKEN_NOT_EXIST:
		return "������δ����devicetoken�ֶ�";
	case MIG_FM_HTTP_INVALID_TIME_FORMAT:
		return "������ʱ���ʽ�Ƿ�";
	case MIG_FM_DB_SAVE_PUSH_CONFIG_FAILED:
		return "���ݿⱣ����������ʧ��";
	case MIG_FM_DB_READ_PUSH_CONFIG_FAILED:
		return "���ݿ��ȡ��������ʧ��";
	case MIG_FM_OTHER_PUSH_SERVICE_CLOSED:
		return "�Է����ͷ���ر�";
	case MIG_FM_OTHER_ANTI_HARASSMENT:
		return "�Է�����������";
	case MIG_FM_DB_ACCESS_FAILED:
		return "���ݿ����ʧ��";
	case MIG_FM_PUSH_MSG_FAILED:
		return "������Ϣʧ��";
	case MIG_FM_MSG_LIST_EMPTY:
		return "��Ϣ�б�Ϊ��";
	case MIG_FM_SOURCE_SESSION_NO_VAILED:
		return "������δ������������ʾ��";
	case MIG_FM_USERNAME_NO_VAILED:
		return "������δ�����û���";
	case MIG_FM_PASSWORD_NO_VAILED:
		return "������δ��������";
	case MIG_FM_USER_EXITS:
		return "�û��Ѿ�����";
	case MIG_FM_HTTP_COMMENT_INVALID:
		return "�����������ֶ���Ч";
	case MIG_FM_HTTP_FROMID_INVALID:
		return "��������ʼID��Ч";
	case MIG_FM_HTTP_COUNT_INVALID:
		return "�����з�ҳ������Ч";
	case MIG_FM_HTTP_PLAT_INVALID:
		return "��¼ƽ̨��Ϣ����";
	case MIG_FM_HTTP_USERCHECK_INVALID:
		return "�û������������";
	case MIG_FM_HTTP_USERINFO_INVALID:
		return "��Ч�û���Ϣ";
	case MIG_FM_HTTP_JSON_ERROR:
		return "JSON��ʽ����";
	default:
		return "δ֪������";
	}
}
