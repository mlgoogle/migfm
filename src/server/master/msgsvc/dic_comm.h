/*
 * dic_comm.h
 *
 *  Created on: 2015年1月22日
 *      Author: mac
 */

#ifndef DIC_COMM_H_
#define DIC_COMM_H_
#include "basic/basictypes.h"
#include "config/config.h"
#include <list>

namespace msgsvc_logic{
class DicComm{
public:
	DicComm();
	~DicComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetNewCount(const int64 tid, int64& new_msg);
};

}




#endif /* DIC_COMM_H_ */
