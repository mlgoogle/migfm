/*
 * pub_dic_comm.h
 *
 *  Created on: 2015年2月1日
 *      Author: pro
 */

#ifndef PUB_DIC_COMM_H_
#define PUB_DIC_COMM_H_

#include "logic_infos.h"
#include "config/config.h"
#include <list>
#include <map>

namespace basic_logic{

class PubDicComm{
public:
	PubDicComm();
	~PubDicComm();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static void GetColllectList(const int64 uid,std::list<std::string>& list);
	//static void GetBatchMusicInfo(std::list<base_logic::MusicInfo>& list);
};
}



#endif /* PUB_DIC_COMM_H_ */
