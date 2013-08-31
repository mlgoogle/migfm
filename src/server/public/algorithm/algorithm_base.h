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
	//计算一个数出现重复的可能性，算出最大范围
	static int32 GetTotalForNum(int32 num,int32 n);
	//根据顺序分配每个纬度的歌曲
	static void AllocationLatitudeMusicNum(const int32 mood_index,
		      int32& mood_num,const int32 scenes_index,int32& scenes_num,
			  const int32 channel_index, int32& channel_num,const int32 num);
};

}
#endif