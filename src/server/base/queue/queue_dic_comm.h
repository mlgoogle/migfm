/*
 * queue_dic_comm.h
 *
 *  Created on: 2015年1月3日
 *      Author: kerry
 */

#ifndef QUEUE_DIC_COMM_H_
#define QUEUE_DIC_COMM_H_
#include "basic/basic_info.h"
#include <list>

namespace base_queue{

class QueueDicComm{
public:
	QueueDicComm();
	~QueueDicComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();

public:
	static bool AddBlockQueueMessage(const std::string& key,const std::string& str);

	static bool GetBlockQueueMessage(const std::string& key,std::list<std::string>& list,
			const int32 count = 1);

	static bool GetBlockQueueMessage(const std::string& key,std::string& str);
};
}


#endif /* QUEUE_DIC_COMM_H_ */
