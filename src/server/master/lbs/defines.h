/*
 * defines.h
 *
 *  Created on: 2013-7-15
 *      Author: huaiyu
 */

#ifndef _DEFINES_H_
#define _DEFINES_H_

namespace mig_lbs {

enum ErrCode {
	ERR_SUCCESS				= 0,
	ERR_UNKNOWN				= -1,
	ERR_CAN_NOT_CONNECT		= -2,
	ERR_PARSE_FAILED		= -3,
	ERR_STD_EXCEPTION		= -4,
};

} // mig_lbs

#endif /* _DEFINES_H_ */
