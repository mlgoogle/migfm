/*
 * behavior.h
 *
 *  Created on: 2015年5月17日
 *      Author: kerry
 *  Time: 下午10:43:11
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */

#ifndef MIGFM_BEHAVIOR_H_
#define MIGFM_BEHAVIOR_H_



namespace base_logic{

class BehaviorEngine{
public:
	BehaviorEngine();
	virtual ~BehaviorEngine();
private:
	void RecordUserListenBehavior(const int64 uid,const int64 songid,
			const int32 dimension,const char* mode);

private:
	void RecordBehaviorFile(const std::string& path,const int64 uid,
			const char* str,const size_t length);
private://序列化
	char* BinarySerialization(void* data,int len,...); //二级制序列化


};

}


#endif /* BEHAVIOR_H_ */
