#ifndef _MIG_FM_PLUBLIC_ALGORITHM_BASE_H__
#define _MIG_FM_PLUBLIC_ALGORITHM_BASE_H__
#include "basic/basictypes.h"
#include "basic/radom_in.h"

namespace algorithm{

class AlgorithmBase
{
public:
	AlgorithmBase(){};
	virtual ~AlgorithmBase();
	static void Init();
	static void Dest();
public:
	//����һ���������ظ��Ŀ����ԣ�������Χ
	static int32 GetTotalForNum(int32 num,int32 n);
	//����˳�����ÿ��γ�ȵĸ���
	static void AllocationLatitudeMusicNum(const int32 mood_index,
		      int32& mood_num,const int32 scenes_index,int32& scenes_num,
			  const int32 channel_index, int32& channel_num,const int32 num);
	//��ĳ������Χ������ȡn�����ظ�����
	static void NotRepeatedNum(int total_num,int* num,int& n);
private:
	//static base::MigRadomIn*              radom_num_;
};

}
#endif