#include "music_logic.h"
#include "whole_manager.h"
#include "db_comm.h"
#include "music_dic_comm.h"
#include "logic/pub_db_comm.h"
#include "logic/pub_dic_comm.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "config/config.h"
#include "common.h"

namespace musicsvc_logic{

Musiclogic*
Musiclogic::instance_=NULL;

Musiclogic::Musiclogic(){
   if(!Init())
      assert(0);
}

Musiclogic::~Musiclogic(){
}

bool Musiclogic::Init(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL)
		return false;
	r = config->LoadConfig(path);
	basic_logic::PubDBComm::Init(config->mysql_db_list_);
	basic_logic::PubDicComm::Init(config->redis_list_);

	musicsvc_logic::MusicDicComm::Init(config->redis_list_);


	musicsvc_logic::DBComm::Init(config->mysql_db_list_);
	musicsvc_logic::CacheManagerOp::GetWholeManager();

	//读取信息
	musicsvc_logic::CacheManagerOp::FetchAvailableMusicInfo();

	//
	musicsvc_logic::CacheManagerOp::FetchDimensionMusic();

	musicsvc_logic::CacheManagerOp::GetWholeManager()->CreateRadomin();

    return true;
}

Musiclogic*
Musiclogic::GetInstance(){

    if(instance_==NULL)
        instance_ = new Musiclogic();

    return instance_;
}



void Musiclogic::FreeInstance(){
    delete instance_;
    instance_ = NULL;
}

bool Musiclogic::OnMusicConnect(struct server *srv,const int socket){

    return true;
}



bool Musiclogic::OnMusicMessage(struct server *srv, const int socket, const void *msg,const int len){
	const char* packet_stream = (char*)(msg);
	std::string http_str(packet_stream,len);
	std::string error_str;
	int error_code = 0;
	LOG_MSG2("%s",packet_stream);

	scoped_ptr<base_logic::ValueSerializer> serializer(base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP,&http_str));


	netcomm_recv::NetBase*  value = (netcomm_recv::NetBase*)(serializer.get()->Deserialize(&error_code,&error_str));
	if(value==NULL){
		error_code = STRUCT_ERROR;
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	scoped_ptr<netcomm_recv::HeadPacket> packet(new netcomm_recv::HeadPacket(value));
	int32 type = packet->GetType();

	switch(type){
	case MUSIC_GAIN_COLLECT_LIST:
		OnCollectList(srv,socket,value);
		break;
	case MUSIC_GAIN_DIMENSION_LIST:
		OnDimensionList(srv,socket,value);
		break;
	case MUSIC_GAIN_DIMENSIONS_LIST:
		OnDimensionsList(srv,socket,value);
		break;
	case MUSIC_GAIN_SET_COLLECT:
		OnSetCollection(srv,socket,value);
		break;
	case MUSIC_GAIN_DEL_COOLECT:
		OnDelCollection(srv,socket,value);
		break;
	}
    return true;
}

bool Musiclogic::OnMusicClose(struct server *srv,const int socket){

    return true;
}



bool Musiclogic::OnBroadcastConnect(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}

bool Musiclogic::OnBroadcastMessage(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}



bool Musiclogic::OnBroadcastClose(struct server *srv, const int socket){

    return true;
}

bool Musiclogic::OnIniTimer(struct server *srv){

    return true;
}



bool Musiclogic::OnTimeout(struct server *srv, char *id, int opcode, int time){

    return true;
}

bool Musiclogic::OnDimensionList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg,const int len){
	scoped_ptr<netcomm_recv::Dimension> dimension(new netcomm_recv::Dimension(netbase));
	int error_code = dimension->GetResult();
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}

	MUSICINFO_MAP music_list;

	musicsvc_logic::CacheManagerOp::GetWholeManager()->GetDimensionList(dimension->dimension_name(),
			dimension->dimension_sub_id(),music_list);
	scoped_ptr<netcomm_send::MusicList> smusiclist(new netcomm_send::MusicList());
	for(MUSICINFO_MAP::iterator it = music_list.begin();it!=music_list.end();){
		base_logic::MusicInfo info = it->second;
		base_logic::DictionaryValue* dict = info.Release();
		if(dict!=NULL)
			smusiclist->set_list(dict);
		music_list.erase(it++);
	}

	send_message(socket,(netcomm_send::HeadPacket*)smusiclist.get());
	return true;
}

bool Musiclogic:: OnDimensionsList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
		const void* msg,const int len){
	scoped_ptr<netcomm_recv::Dimensions> dimension(new netcomm_recv::Dimensions(netbase));
	int error_code = dimension->GetResult();
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}

	MUSICINFO_MAP music_list;
	int index = dimension->ChannelDimension().dimension_index();
	if(dimension->ChannelDimension().dimension_index()!=-1)
		musicsvc_logic::CacheManagerOp::GetWholeManager()->GetDimensionList(dimension->ChannelDimension().dimension_alias_name(),
				dimension->ChannelDimension().dimension_id(),music_list,dimension->Num());
	if(dimension->MoodDimension().dimension_index()!=-1)
		musicsvc_logic::CacheManagerOp::GetWholeManager()->GetDimensionList(dimension->MoodDimension().dimension_alias_name(),
				dimension->MoodDimension().dimension_id(),music_list,dimension->Num());
	if(dimension->ScensDimension().dimension_index()!=-1)
		musicsvc_logic::CacheManagerOp::GetWholeManager()->GetDimensionList(dimension->ScensDimension().dimension_alias_name(),
				dimension->ScensDimension().dimension_id(),music_list,dimension->Num());

	scoped_ptr<netcomm_send::MusicList> smusiclist(new netcomm_send::MusicList());
	for(MUSICINFO_MAP::iterator it = music_list.begin();it!=music_list.end();){
		base_logic::MusicInfo info = it->second;
		base_logic::DictionaryValue* dict = info.Release();
		if(dict!=NULL)
			smusiclist->set_list(dict);
		music_list.erase(it++);
	}

	send_message(socket,(netcomm_send::HeadPacket*)smusiclist.get());
	return true;
}

bool Musiclogic::OnCollectList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg,const int len){
	scoped_ptr<netcomm_recv::Collect> collect(new netcomm_recv::Collect(netbase));
	int error_code = collect->GetResult();
	int64 group_id = 0;
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}

	//获取红心歌单
	MUSICINFO_MAP music_list;

	musicsvc_logic::CacheManagerOp::GetWholeManager()->GetCollectList(collect->uid(),music_list);
	scoped_ptr<netcomm_send::MusicList> smusiclist(new netcomm_send::MusicList());
	for(MUSICINFO_MAP::iterator it = music_list.begin();it!=music_list.end();){
		base_logic::MusicInfo info = it->second;
		base_logic::DictionaryValue* dict = info.Release();
		if(dict!=NULL)
			smusiclist->set_list(dict);
		music_list.erase(it++);
	}

	send_message(socket,(netcomm_send::HeadPacket*)smusiclist.get());
	return true;
}

bool Musiclogic::OnSetCollection(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg,const int len){
	scoped_ptr<netcomm_recv::SetCollect> collect(new netcomm_recv::SetCollect(netbase));
	int error_code = collect->GetResult();
	int64 group_id = 0;
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	//设置
	base_logic::MusicInfo musicinfo;
	musicinfo.set_id(collect->songid());
	musicinfo.set_class(collect->dimemsion_id());
	musicinfo.set_class_name(collect->dimension_alais());
	musicsvc_logic::CacheManagerOp::GetWholeManager()->SetCollectSong(collect->uid(),musicinfo);

	scoped_ptr<netcomm_send::HeadPacket> head(new netcomm_send::HeadPacket());
	head->set_status(1);
	send_message(socket,(netcomm_send::HeadPacket*)head.get());
	return true;
}

bool Musiclogic::OnDelCollection(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
    		const void* msg,const int len){
	scoped_ptr<netcomm_recv::DelCollect> delcollect(new netcomm_recv::DelCollect(netbase));
	int error_code = delcollect->GetResult();
	if(error_code!=0){
		//发送错误数据
		send_error(error_code,socket);
		return false;
	}
	musicsvc_logic::CacheManagerOp::GetWholeManager()->DelCollectSong(delcollect->uid(),delcollect->songid());
	scoped_ptr<netcomm_send::HeadPacket> head(new netcomm_send::HeadPacket());
	head->set_status(1);
	send_message(socket,(netcomm_send::HeadPacket*)head.get());
	return true;
}

bool Musiclogic::OnHateList(struct server *srv,const int socket,netcomm_recv::NetBase* netbase,
       		const void* msg,const int len){
	return true;
}

}

