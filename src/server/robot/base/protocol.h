#ifndef __MIG_CHAT_BASE_COMM_HEAD_H__
#define __MIG_CHAT_BASE_COMM_HEAD_H__
#include "comm_head.h"
#include "protocol/data_packet.h"
#include "basic/basictypes.h"
#include "basic/radom_in.h"


#define  BUILDHEAD(DATA_LENGTH)                             \
	data_length = DATA_LENGTH;                              \
	packet_length = data_length + PACKET_HEAD_LENGTH;         \
	packet = new char[packet_length];                       \
	memset(packet,'\0',packet_length);                      \
	data = packet + PACKET_HEAD_LENGTH;                       \
	packet::DataOutPacket out(false,packet_length);         \
	out.Write32(packet_length);                             \
	out.Write32(operate_code);                              \
	out.Write32(data_length);                               \
	out.Write32(current_time);                              \
	out.Write16(msg_type);                                  \
    out.Write8(is_zip);                                     \
    out.Write64(msg_id);                                    \
	out.Write32(reserverd)


#define BUILDPACKHEAD()                                             \
	packet::DataInPacket in((char*)packet_stream,packet_length);    \
	(*packhead)->packet_length = in.Read32();                       \
	(*packhead)->operate_code = in.Read32();                      \
	(*packhead)->data_length = in.Read32();                         \
	(*packhead)->current_time = in.Read32();                        \
	(*packhead)->msg_type = in.Read16();                            \
	(*packhead)->is_zip = in.Read8();                               \
	(*packhead)->msg_id = in.Read64();                              \
	(*packhead)->reserverd = in.Read32()                            \


#define _MAKE_HEAD(head, _packet_length, _operate_code,_data_length,_msg_type,_is_zip,_reserverd)   \
	do {                                                                             \
	head.packet_length = _packet_length;                                             \
	head.operate_code = _operate_code;                                             \
	head.data_length = _data_length;                                                 \
	head.current_time = time(NULL);                                                  \
	head.msg_type = _msg_type;                                                       \
	head.is_zip  = _is_zip;                                                          \
    head.msg_id = base::SysRadom::GetInstance()->GetRandomID();                      \
	head.reserverd = _reserverd;                                                     \
	} while (0)


#define MAKE_FAIL_HEAD(head, _operate_code,_msg_type,_is_zip,_reserverd)   \
	_MAKE_HEAD (head, PACKET_HEAD_LENGTH, _operate_code, 0,_msg_type,_is_zip,_reserverd)

#define MAKE_HEAD(head, _operate_code,_msg_type,_is_zip,_reserverd)    \
	_MAKE_HEAD (head, 0, _operate_code, 0,_msg_type,_is_zip,_reserverd)

class ProtocolPack
{
public:
	static bool PackStream(const struct PacketHead* packhead,void** packet_stream,
		int32* packet_stream_length);

	static bool UnpackStream(const void *packet_stream, int len, struct PacketHead**packhead /* delete */);

	static void HexEncode(const void* bytes, size_t size);

	static void DumpPacket (const struct PacketHead *packhead);
};
#endif
