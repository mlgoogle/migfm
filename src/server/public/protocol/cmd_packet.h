#ifndef MIG_FM_PUBLIC_PROTOCOL_CMD_PACKET_H__
#define MIG_FM_PUBLIC_PROTOCOL_CMD_PACKET_H__

#include "basic/basictypes.h"
namespace packet{

class ChatPack{
public:
	static bool PacketStream(const struct PacketHead* packet_head,
		                     void **packet_stream,int32 *packet_stream_length);

	static bool UnPacketStream(const void *packet_stream, int packet_stream_length, 
	                           struct PacketHead **packhead);

	static void DumpPacket (const struct PacketHead *packhead);

	static void HexEncode(const void* bytes, int32 size);
};

}
#endif