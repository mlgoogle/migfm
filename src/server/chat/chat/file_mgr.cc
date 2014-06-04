#include "file_mgr.h"
#include "dic_comm.h"
#include "db_comm.h"
#include "chat_cache_manager.h"
#include "logic_unit.h"
#include "base/logic_comm.h"
#include "base/error_code.h"
#include "json/json.h"
#include <sstream>

namespace chat_logic{

FileMgr::FileMgr(){

}

FileMgr:: ~FileMgr(){

}

bool FileMgr::OnGroupSound(struct server *srv, int socket, struct PacketHead *packet,
	           const void *msg/* = NULL*/, int len/* = 0*/){
	struct MultiSoundSend* multi_sound_send = (struct MultiSoundSend*)packet;
	//声音文件不做任何缓存
	//服务端写入数据库
	chat_logic::PlatformChatCacheManager* pc = CacheManagerOp::GetPlatformChatMgrCache();

	std::string path =multi_sound_send->sound_path;
	std::string full_path = sound_url+path;
	struct MultiSoundRecv multi_sound_recv;
	MAKE_HEAD(multi_sound_recv, MULTI_SOUND_RECV,MEDIA_TYPE,0,multi_sound_send->reserverd);
	multi_sound_recv.platform_id = multi_sound_send->platform_id;
	multi_sound_recv.multi_id = multi_sound_send->multi_id;
	multi_sound_recv.send_user_id = multi_sound_send->send_user_id;
	multi_sound_recv.sound_path = full_path;

	pc->SendMeetingMessage(multi_sound_recv.platform_id,multi_sound_recv.multi_id,
			multi_sound_send->session,&multi_sound_recv);

	return true;
}

}
