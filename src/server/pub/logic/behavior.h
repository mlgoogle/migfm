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

#include "basic/basictypes.h"
#include <string>
#include <sstream>
namespace base_logic{


enum BEHAVIOR_TYPE{
	BEHAVIOR_LIST_MUSIC_TYPE =  1000
};
#define PACKET_HEAD_LENGTH (sizeof(int32) * 3 + sizeof(64))

struct PacketHead{
	int32 packet_length;
	int32 operate_code;
	int32 data_length;
	int64 current_time;
};

//
#define LISTEN_MUSICBEHAVIOR_SIZE (sizeof(int64) * 2 + sizeof(int32) + 32)
struct ListenMusicBehavior:public PacketHead{
	int64 uid;
	int64 songid;
	int32 dimension_sub_id;
	char dimension_name[32];
};

#define BUILDPACKHEAD()                                             \
	packet::DataInPacket in((char*)packet_stream,packet_length);    \
	(*packhead)->packet_length = in.Read32();                       \
	(*packhead)->operate_code = in.Read32();                      \
	(*packhead)->data_length = in.Read32();                         \
	(*packhead)->current_time = in.Read64();                        \



#define  RECORD_BUILDHEAD(DATA_LENGTH)					\
	data_length = DATA_LENGTH;                          \
	packet_length = data_length + PACKET_HEAD_LENGTH;   \
	packet = new char[packet_length];                   \
	memset(packet,'\0',packet_length);                  \
	data = packet + PACKET_HEAD_LENGTH;                 \
	packet::DataOutPacket out(false,packet_length);     \
	out.Write32(packet_length);                         \
	out.Write32(operate_code);                          \
	out.Write32(data_length);                           \
	out.Write64(current_time);                          \

#define _MAKE_HEAD(head, _packet_length, _operate_code,_data_length)   \
	do {                                                                             \
	head.packet_length = _packet_length;                                             \
	head.operate_code = _operate_code;                                               \
	head.data_length = _data_length;                                                 \
	head.current_time = time(NULL);                                                  \
	} while (0)                                                                      \


#define MAKE_HEAD(head, _operate_code)    \
	_MAKE_HEAD (head, 0, _operate_code, 0)

class BehaviorEngine{
public:
	static BehaviorEngine* Instance();
	static void FreeInstance();

public:
	BehaviorEngine();
	virtual ~BehaviorEngine();
public:
	void RecordUserListenBehavior(const int64 uid,const int64 songid,
			const int32 dimension,const char* mode);
private://序列化
	void RecordBehaviorFile(const std::string& path,const int64 uid,
			const char* str,const size_t length);

	bool BinarySerialization(const struct PacketHead* packhead,void** packet_stream,
			int32* packet_stream_length); //二级制序列化

private:
	 static BehaviorEngine*      instance_;
};

}


#endif /* BEHAVIOR_H_ */
