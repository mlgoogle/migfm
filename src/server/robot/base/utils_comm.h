/*
 * util_comm.h
 *
 *  Created on: 2015年5月12日
 *      Author: kerry
 *  Time: 下午8:50:16
 *  Project: migfm
 *  Year: 2015
 *  Instruction：封装一些机器人涉及的基础方式
 */


#ifndef UTIL_COMM_H_
#define UTIL_COMM_H_

#include "base/comm_head.h"
#include "base/protocol.h"
#include "basic/basictypes.h"

namespace robot_logic{

class SomeUtils{
public:
	static bool SendMessage (int socket, struct PacketHead *packet,
		const char *file, long line);

	static bool SendHeadMessage (int socket, int32 operate_code, int16 msg_type,
		int8 is_zip,int session, const char *file, long line);

	static bool SendErrorCode(int socket,int32 operate_code,int16 msg_type,int8 is_zip,
			int32 session,const int32 error_code,const char *file,long line);

	static void CloseSocket(int socket);

	static bool GetRobotLbsPos(const double& latitude,const double& longitude,double& robot_latitude,
			double& robot_longitude);
};

}

/*
#define sendbytes(socket,bytes,len)     \
	robot_logic::SomeUtils::SendBytes(socket,bytes,len,__FILE__,__LINE__)
*/

#define sendmessage(socket,packet) \
		robot_logic::SomeUtils::SendMessage(socket,packet,__FILE__,__LINE__)

#define sendheadmsg(socket,operate_code,msg_type,is_zip,session)  \
	robot_logic::SomeUtils::SendHeadMessage(socket,operate_code,msg_type,is_zip,session,__FILE__,__LINE__)

#define senderror(socket,operate_code,is_zip,session,error_code)  \
	robot_logic::SomeUtils::SendErrorCode(socket,operate_code,ERROR_TYPE,is_zip,session,error_code,__FILE__,__LINE__)

#define closeconnect(socket) \
	robot_logic::SomeUtils::CloseSocket(socket)

#define closelockconnect(socket) \
	shutdown(socket, SHUT_RDWR);

#define sendrobotmssage(robot_info,packet) \
	robot_info.set_send_last_time(time(NULL));\
	r = robot_logic::SomeUtils::SendMessage(robot_info.socket(),packet,__FILE__,__LINE__)

#define sendschdulermessage(scheduler_info,packet) \
	r = robot_logic::SomeUtils::SendMessage(scheduler_info.socket(),packet,__FILE__,__LINE__)




#endif /* UTIL_COMM_H_ */
