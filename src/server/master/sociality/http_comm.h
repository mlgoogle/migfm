/*
 * http_comm.h
 *
 *  Created on: 2013-8-11
 *      Author: huaiyu
 */

#ifndef _HTTP_COMM_H_
#define _HTTP_COMM_H_

#include <string>

namespace mig_sociality {

class HttpComm {
public:
	static bool PushMessage(const std::string &device_token, const std::string &msg,
			int badge=1, const std::string &sound="");
};

} /* namespace mig_sociality */
#endif /* _HTTP_COMM_H_ */
