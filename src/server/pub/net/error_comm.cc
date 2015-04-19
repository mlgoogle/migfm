/*
 * error_comm.cc
 *
 *  Created on: 2014年12月5日
 *      Author: kerry
 */
#include "net/error_comm.h"

const char*
miyo_strerror(int error_code){
	switch (error_code) {
	  case STRUCT_ERROR:
		  return STRUCT_ERROR_STR;
	  case TYPE_LACK:
		  return TYPE_LACK_STR;
	  case PLATFORM_LACK:
		  return PLATFORM_LACK_STR;
	  case UID_LACK:
		  return UID_LACK_STR;
	  case TOKEN_LACK:
		  return TOKEN_LACK_STR;
	  case MUSIC_TYPE_LACK:
		  return MUSIC_TYPE_LACK_STR;
	  case MUSIC_TYPE_ID_LACK:
		  return MUSIC_TYPE_ID_LACK_STR;
	  case MESSAGE_ID_LACK :
		  return MESSAGE_ID_LACK_STR;
	  case GROUP_ID_LACK:
		  return GROUP_ID_LACK_STR;
	  case TID_LACK:
		  return TID_LACK_STR;
	  default:
		  return UNKOWN_ERROR_STR;
	}
}


