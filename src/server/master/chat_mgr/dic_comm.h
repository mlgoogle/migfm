#ifndef _MASTER_PLUGIN_CHAT_MGR_DIC_COMM__H__
#define _MASTER_PLUGIN_CHAT_MGR_DIC_COMM__H__


#include "storage/storage.h"
#include "basic/basictypes.h"
#include "basic/basic_info.h"
#include <list>

namespace chat_storage{

class MemComm{
public:
	MemComm(){}
	virtual ~MemComm(){}
	static void Init(std::list<base::ConnAddr>& addrlist);
	static void Dest();
public:
	static bool GetUserToken(const std::string& platform_id,
			const std::string& uid,std::string& token);

private:
	static base_storage::DictionaryStorageEngine* engine_;
};


}

#endif
