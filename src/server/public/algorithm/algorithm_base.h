#ifndef _MIG_FM_PLUBLIC_ALGORITHM_BASE_H__
#define _MIG_FM_PLUBLIC_ALGORITHM_BASE_H__
#include "basic/basictypes.h"

namespace algorithm{

class AlgorithmBase
{
public:
	AlgorithmBase(){};
	virtual ~AlgorithmBase();
public:
	//����һ���������ظ��Ŀ����ԣ�������Χ
	static int32 GetTotalForNum(int32 num,int32 n);
	//����˳�����ÿ��γ�ȵĸ���
	static void AllocationLatitudeMusicNum(const int32 mood_index,
		      int32& mood_num,const int32 scenes_index,int32& scenes_num,
			  const int32 channel_index, int32& channel_num,const int32 num);
};

}
#endif