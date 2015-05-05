/*
 * http_formate.h
 *
 *  Created on: 2015年4月22日
 *      Author: pro
 */

#ifndef HTTP_FORMATE_H_
#define HTTP_FORMATE_H_

#include "basic/basictypes.h"
#include <string>
#include <sstream>
namespace http{
class HttpFormate{
public:
	HttpFormate(){}
	virtual ~HttpFormate(void){}

	static void SetHttpGetMethod(int32 type,const char* query,
			std::string& content);

	static void SetHttpPostMethod(const int32 type,const char* data,
			std::string& content);

	static void SetHttpResponse(const std::string& response);
};

}



#endif /* HTTP_FORMATE_H_ */
