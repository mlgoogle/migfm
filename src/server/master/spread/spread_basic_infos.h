#ifndef _MASTER_PLUGIN_MANAGER_SPREAD_BASIC_INFOS_H__
#define _MASTER_PLUGIN_MANAGER_SPREAD_BASIC_INFOS_H__

namespace base{

class WeiXin{
public:
	class WeiXinMenuInfo{
		explicit WeiXinMenuInfo(const int64 id,const std::string& name,const std::string& key,
				const std::string& url);
		explicit WeiXinMenuInfo();
	};
};
}
#endif
