#ifndef _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#define _MASTER_PLUGIN_USR_MGR_DB_COMM_H__
#include "storage/storage.h"
#include "basic/basic_info.h"
#include <list>
#include <vector>
namespace storage{

class DBComm{
public:
	DBComm(){}
	virtual ~DBComm(){}
	static base_storage::DBStorageEngine *GetConnection();
public:
	static void Init(std::list<base::ConnAddr>& addrlist);
	
	static void Dest();

	static bool GetChannelInfo(std::vector<base::ChannelInfo>& channel,int& num);
	static bool GetDescriptionWord(std::list<base::WordAttrInfo>& word_list,int flag);
private:
	static std::list<base::ConnAddr>  addrlist_;
};

}
#endif