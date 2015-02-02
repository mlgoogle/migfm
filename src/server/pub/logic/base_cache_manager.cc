/*
 * base_cache_manager.cc
 *
 *  Created on: 2015年2月1日
 *      Author: pro
 */

#include "base_cache_manager.h"

namespace base_logic{

BaseWholeManager::~BaseWholeManager(){
	DeinitThreadrw(lock_);
}

void BaseWholeManager::Init(){
	//
	InitThreadrw(&lock_);
	//根据MP3URL 来读取音乐信息
	//基本信息，附属信息，URL，歌词
}
}


