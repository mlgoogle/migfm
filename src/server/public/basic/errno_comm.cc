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
		default:
			return "δ֪������";

	}
}