/*
 * lbs_dic_comm.h
 *
 *  Created on: 2015年3月6日
 *      Author: pro
 */

#ifndef LBS_DIC_COMM_H_
#define LBS_DIC_COMM_H_

#include "storage/storage.h"
#include "basic/basic_info.h"
#include "basic/basictypes.h"
#include <list>

namespace base_lbs{
class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

	//static bool SetString(const char* key,const size_t key_len,
		  //                const char* data, size_t len);

	//static bool GetString(const char* key,const size_t key_len,
		//                  char** data,size_t* len);

	static bool SetLocation(const double latitude,const double longitude,const char* data,
			size_t len);

	static bool GetLocation(const double latitude,const double longitude,char** data,
			size_t* len);

	static bool DelLocation(const double latitude,const double longitude);
private:
	static base_storage::DictionaryStorageEngine* engine_;
};

}


#endif /* LBS_DIC_COMM_H_ */
