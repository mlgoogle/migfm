/*
 * util_comm.cc
 *
 *  Created on: 2015年5月12日
 *      Author: kerry
 *  Time: 下午9:06:17
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */

#include "utils_comm.h"
#include "logic/logic_comm.h"
#include "storage/storage.h"
#include "basic/basic_util.h"
#include "log/mig_log.h"
#include "basic/radom_in.h"
#include <sstream>
#include <sys/socket.h>
#include <time.h>

namespace robot_logic{

bool SomeUtils::SendMessage(int socket, struct PacketHead *packet,
							 const char *file, long line){

	bool r;
	void *packet_stream = NULL;
	int32_t packet_stream_length = 0;
	int ret = 0;
	bool  r1 = false;
	if (socket <= 0 || packet == NULL)
		return false;

	if (ProtocolPack::PackStream (packet, &packet_stream, &packet_stream_length) == false) {
		LOG_ERROR2 ("Call PackStream failed in %s:%d", file, line);
		r = false;
		goto MEMFREE;
	}

	//LOG_DEBUG2("opcode[%d]\n",packet->operate_code);
	ProtocolPack::DumpPacket(packet);
	ret = base_logic::LogicComm::SendFull (socket, (char *) packet_stream, packet_stream_length);
	//ProtocolPack::HexEncode(packet_stream,packet_stream_length);
	if (ret != packet_stream_length) {
		LOG_ERROR2 ("Sent msg failed in %s:%d", file, line);
		r = false;
		goto MEMFREE;
	} else {
		r = true;
		goto MEMFREE;
	}
	MEMFREE:
	char* stream = (char*)packet_stream;
	if (stream){
		delete[] stream;
		stream = NULL;
	}

	return r;
}

bool SomeUtils::SendHeadMessage(int socket, int32 operate_code, int16 msg_type,
								int8 is_zip,int session,
								const char *file, long line){
	struct PacketHead packet;
	//MAKE_FAIL_HEAD (packet, operate_code,session);
	int64 msg_id = base::SysRadom::GetInstance()->GetRandomID();
	//MAKE_FAIL_HEAD(head, _operate_code,_msg_type,_is_zip,_reserverd)
	MAKE_FAIL_HEAD(packet,operate_code,msg_type,is_zip,session);
	return SendMessage (socket, &packet, file, line);
}

void SomeUtils::CloseSocket(int socket){
	close(socket);
}

bool SomeUtils::SendErrorCode(int socket,int32 operate_code,int16 msg_type,
		                      int8 is_zip,int32 session,const int32 error_code,
							  const char *file,long line){
	/*const char* error_msg = migchat_strerror(error_code);
	struct ChatFailed chat_failed;
	MAKE_HEAD(chat_failed,operate_code,msg_type,is_zip,session);
	//logic::SomeUtils::SafeStrncpy(chat_failed.error_msg.c_str(),chat_failed.error_msg.length(),
		//						error_msg,strlen(error_msg));
	chat_failed.platform_id = 10000;
	chat_failed.error_msg.assign(error_msg);
	return SendMessage(socket,&chat_failed,file,line);
	*/
	return true;
}




}


