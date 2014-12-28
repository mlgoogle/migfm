/*
 * cache_manager.h
 *
 *  Created on: 2014年12月11日
 *      Author: kerry
 */

#ifndef MIGFM_CACHE_MANAGER_H_
#define MIGFM_CACHE_MANAGER_H_
#include "logic_infos.h"

namespace base_logic{

class WholeManager{
private:
	static WholeManager*   whole_manager_;
	WholeManager() {Init();}
	virtual ~WholeManager() {}
public:
	static WholeManager* GetWholeManager(){
		if(whole_manager_==NULL)
			whole_manager_ = new WholeManager();
		return whole_manager_;
	}

	static void FreeWholeManager(){
		delete whole_manager_;
	}
public:
	void Init();
public:
	int64 dimensions_name(const std::string& alias,
			const int64 id,std::string& name);

private:
	void  CreateDimensions(const int64 id,
			const std::string& type,const std::string& name);
private:
	std::map<std::string,base_logic::Dimensions>   dimensions_;
};
}




#endif /* CACHE_MANAGER_H_ */
