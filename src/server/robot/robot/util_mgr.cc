#include "util_mgr.h"
#include "logic_unit.h"
#include "prize_rate.h"
#include "base/comm_head.h"
#include "robot_cache_manager.h"
#include "base/logic_comm.h"

namespace robot_logic{

UtilMgr::UtilMgr(){

}

UtilMgr::~UtilMgr(){

}

bool UtilMgr::OnUserReadyLuckGift(struct server *srv, int socket, struct PacketHead *packet,
	        const void *msg, int len){
	struct NoticeUserReadyGiftLuck* vNoticeUserReadyGiftLuck = (struct NoticeUserReadyGiftLuck*)packet;
	return LuckGiftCalculation(vNoticeUserReadyGiftLuck->share_plat,
			vNoticeUserReadyGiftLuck->uid,vNoticeUserReadyGiftLuck->songid);
}

bool UtilMgr::LuckGiftCalculation(const int32 plat,const int64 uid,const int64 songid){
	//获取此平台的中奖率
	luck_prize prize[3];
	LuckGiftInfoPlatMap plat_map;
	int late_rate = -1;
	bool r = robot_logic::CacheManagerOp::GetCacheManagerOp()->GetLuckGiftInfo(plat,plat_map);
	if(!r)
		return false;
	//设置奖项大小范围
	//MAP为有序集合
	LuckGiftInfoPlatMap::iterator it = plat_map.begin();
	for(int j = 0;it!=plat_map.end();it++,j++){
		prize[j].min = late_rate+1;
		prize[j].max = prize[j].min + it->second->rate() -1;
		prize[j].rate = prize[j].max - prize[j].min + 1;
		prize[j].prize = j+1;
		late_rate = prize[j].max;
		//LOG_DEBUG2("min %d max %d rate %d prize %d infoprize %d",prize[j].min,prize[j].max,
			//	prize[j].rate,prize[j].prize,it->second->prize());
	}
	//抽奖
	int prize_result = robot_logic::CacheManagerOp::GetCacheManagerOp()->GetPrize(prize,3);
	LOG_DEBUG2("prize %d",prize_result);
	prize_result = 1;
	return robot_logic::CacheManagerOp::GetRobotCacheMgr()->SendAssistantLuckGift(10000,uid,
			plat,prize_result,songid);

}

}
