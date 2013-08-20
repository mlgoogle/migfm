/*
*
* Returns: a static string.
*/
#include "errno_comm.h"
const char*
migfm_strerror(int error_code){
	
	switch (error_code){
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
		default:
			return "δ֪������";

	}
}