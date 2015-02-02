/*
 * base_cache_manager.h
 *
 *  Created on: 2015年2月1日
 *      Author: pro
 */

#ifndef BASE_CACHE_MANAGER_H_
#define BASE_CACHE_MANAGER_H_


#include "logic_infos.h"
#include "logic_unit.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"
#include "basic/scoped_ptr.h"
#include "basic/basictypes.h"

namespace base_logic{

class BaseCache{
public:
	MUSICINFO_MAP                        music_info_map_;
	MUSICINFONLIST_MAP                   collect_map_;
	MUSICINFONLIST_MAP                   hate_map_;
};


class BaseWholeManager{ //不单独创建对象，只做父类 //公共信息模块操作
public:
	~BaseWholeManager();
public:
	void Init();
public:
	struct threadrw_t*            lock_;

private:
	//DISALLOW_IMPLICIT_CONSTRUCTORS(BaseWholeManager) //禁止构造函数即创建此对象
};

}


#endif /* BASE_CACHE_MANAGER_H_ */
