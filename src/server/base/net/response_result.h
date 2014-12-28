/*
 * response.h
 *
 *  Created on: 2014年11月17日
 *      Author: kerry
 */

#ifndef _LOGIC_RESPONSE_RESULT_H_
#define _LOGIC_RESPONSE_RESULT_H_
#include "logic/base_values.h"
namespace base_net{

class ResponseResult{
public:
	ResponseResult(){}
	virtual ~ResponseResult(){}
private:
	class BaseManager{
		static bool OnBaseSend(const int socket,base_logic::Value* sub);
	};
public:
	class UserManager{
		static bool  OnUserRegister(const int socket,const std::string& uid,const std::string& username,const std::string& nickname,
			const std::string& gender,const int32& type,const std::string& birthday,const std::string& location,
			const std::string& age,const std::string& head,const std::string& token,const std::string& new_msg_num);
	}
};

}



#endif /* RESPONSE_H_ */
