/*
 * behavior.cc
 *
 *  Created on: 2015年5月17日
 *      Author: kerry
 *  Time: 下午10:43:21
 *  Project: migfm
 *  Year: 2015
 *  Instruction：
 */
#include "behavior.h"
#include "logic/logic_comm.h"
#include "file/file_path.h"
#include "file/file_util.h"
#include "basic/basic_util.h"
#include "protocol/data_packet.h"

namespace base_logic{

BehaviorEngine* BehaviorEngine::instance_ = NULL;

BehaviorEngine* BehaviorEngine::Instance(){
	if(instance_ == NULL){
		instance_ = new BehaviorEngine();
	}
	return instance_;
}

void BehaviorEngine::FreeInstance(){
	delete instance_;
	instance_ = NULL;
}

BehaviorEngine::BehaviorEngine(){

}

BehaviorEngine::~BehaviorEngine(){

}

void BehaviorEngine::RecordUserListenBehavior(const int64 uid,const int64 songid,
		const int32 dimension,const char* mode){
	void *packet_stream = NULL;
	int32 packet_stream_length = 0;
	struct ListenMusicBehavior listen_music;
	MAKE_HEAD(listen_music,BEHAVIOR_LIST_MUSIC_TYPE);
	bool r = BinarySerialization(&listen_music,&packet_stream,&packet_stream_length);
	if(!r)
		return;

	//写入文件
	RecordBehaviorFile("./behavior/music",uid,(char*)packet_stream,packet_stream_length);

	if(packet_stream){
		char* del_packet_stream =(char*) packet_stream;
		delete del_packet_stream;
		del_packet_stream = NULL;
	}
}


bool BehaviorEngine::BinarySerialization(const struct PacketHead* packhead,void** packet_stream,
			int32* packet_stream_length){
	bool r = true;
	int32 packet_length = packhead->packet_length;
	int32 operate_code = packhead->operate_code;
	int32 data_length = packhead->data_length;
	int64 current_time = packhead->current_time;
	char* packet = NULL;
	char* data = NULL;

	switch(operate_code){
		case BEHAVIOR_LIST_MUSIC_TYPE:
		{
			struct ListenMusicBehavior* vListenMusicBehavior =
					(struct ListenMusicBehavior*)packhead;
			RECORD_BUILDHEAD(LISTEN_MUSICBEHAVIOR_SIZE);
			out.Write64(vListenMusicBehavior->uid);
			out.Write64(vListenMusicBehavior->songid);
			out.Write32(vListenMusicBehavior->dimension_sub_id);
			out.WriteData(vListenMusicBehavior->dimension_name,32);
			packet = (char*)out.GetData();
			break;
		}
		default:
			r = false;
			break;
	}

	*packet_stream = (void *)packet;
	if (packhead->packet_length == PACKET_HEAD_LENGTH
		&& packhead->data_length == 0)
		*packet_stream_length = PACKET_HEAD_LENGTH;
	else
		*packet_stream_length = packet_length;
	return r;
}

void BehaviorEngine::RecordBehaviorFile(const std::string& path,const int64 uid,
			const char* str,const size_t length){
	time_t current_time = time(NULL);
	std::stringstream os;
	struct tm* local_time = localtime(&current_time);
	os<<path<<"/"<<(local_time->tm_year+1900)
			<<(local_time->tm_mon+1)<<(local_time->tm_mday);
	std::string current_dir = os.str();
	LOG_DEBUG2("%s",current_dir.c_str());
	file::FilePath current_dir_path(current_dir);
	if (!file::DirectoryExists(current_dir_path)){
		file::CreateDirectory(current_dir_path);
	}
	file::FilePath file_path(current_dir+"/"+base::BasicUtil::StringUtil::Int64ToString(uid)+".beh");
	file::WriteFile(file_path,str,length);

}

}


