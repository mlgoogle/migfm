#ifndef _CHAT_FILE_MGR_H__
#define _CHAT_FILE_MGR_H__

#include "chat_basic_infos.h"
#include "common.h"
#include "base/comm_head.h"
#include "basic/basictypes.h"
#include <string>

namespace chat_logic{

class FileMgr{
public:
	FileMgr();
	virtual ~FileMgr();
public:
	bool OnGroupSound(struct server *srv, int socket, struct PacketHead *packet,
	           const void *msg = NULL, int len = 0);

};

}
#endif
