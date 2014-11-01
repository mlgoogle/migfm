#ifndef _RESTFUL_HTTP_API_H__
#define _RESTFUL_HTTP_API_H__
#include "http/http_packet.h"

namespace base_net{

class MYHttpApi{
public:
	MYHttpApi(){};
	virtual ~MYHttpApi(){};

public:
	class UserManager{
		static bool OnUserRegister(packet::HttpPacket& packet,std::string& username,
				std::string& password,std::string& nickname,std::string& source,
				std::string& session,std::string& sex,std::string& borthday,std::string& location,
				std::string& head,int& err);

		static bool OnUserUpdate(packet::HttpPacket& packet,std::string& uid,
				         std::string& nickname,std::string& gender,
				         std::string& birthday,int& err);

		static bool OnUserLogin(packet::HttpPacket& packet,std::string& username,
				std::string& password,std::string& clientid,int& err);

		//static bool OnBindPush(packet::HttpPacket& packet,std::string& uid,std::string& channel,
			//	std::string& userid,int& err);
	};

};
}
#endif
