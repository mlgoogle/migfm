#include "msg_logic.h"
#include "msg_basic_info.h"
#include "db_comm.h"
#include "queue/block_msg_queue.h"
#include "logic/pub_db_comm.h"
#include "logic/cache_manager.h"
#include "pushmsg/push_connector.h"
#include "basic/basic_util.h"
#include "common.h"
#include <string>
#include <list>

#define TIME_GET_BLOCK_QUEUE    10001//消息检测
namespace msgsvc_logic{

Msglogic*
Msglogic::instance_=NULL;

Msglogic::Msglogic(){
   if(!Init())
      assert(0);
}

Msglogic::~Msglogic(){
}

bool Msglogic::Init(){
	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}
	r = config->LoadConfig(path);
	msgsvc_logic::DBComm::Init(config->mysql_db_list_);
	basic_logic::PubDBComm::Init(config->mysql_db_list_);
	base_push::PushConnectorEngine::Create(base_push::IMPL_BAIDU);
	base_push::PushConnectorEngine::GetPushConnectorEngine()->Init(config->mysql_db_list_);
	base_logic::WholeManager::GetWholeManager()->Init(config->redis_list_);
    return true;
}

Msglogic*
Msglogic::GetInstance(){

    if(instance_==NULL)
        instance_ = new Msglogic();

    return instance_;
}



void Msglogic::FreeInstance(){
    delete instance_;
    instance_ = NULL;
}

bool Msglogic::OnMsgConnect(struct server *srv,const int socket){

    return true;
}



bool Msglogic::OnMsgMessage(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}

bool Msglogic::OnMsgClose(struct server *srv,const int socket){

    return true;
}



bool Msglogic::OnBroadcastConnect(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}

bool Msglogic::OnBroadcastMessage(struct server *srv, const int socket, const void *msg,const int len){

    return true;
}



bool Msglogic::OnBroadcastClose(struct server *srv, const int socket){

    return true;
}

bool Msglogic::OnIniTimer(struct server *srv){
	srv->add_time_task(srv,"msg",TIME_GET_BLOCK_QUEUE,10,-1);
    return true;
}



bool Msglogic::OnTimeout(struct server *srv, char *id, int opcode, int time){
	switch(opcode){
		case TIME_GET_BLOCK_QUEUE:
			GetPushMessageQueue();
			break;
	}

    return true;
}

void Msglogic::GetPushMessageQueue(){
	//读取redis
	std::list<base_queue::BlockMsg*> list;
	//GIVINGSONGMAP givingsong_list;
	std::string key="miyo:0";
	base_logic::WholeManager::GetWholeManager()->GetBlockMsgQueue(key,base_queue::TYPE_JSON,list);
	while(list.size()>0){
		base_queue::BlockMsg* msg = list.front();
		if (msg->MsgType()==0)
			ResolveSayHello(msg);
		else if(msg->MsgType()==1)
			ResolveGiving(msg);

		list.pop_front();
	}
}

void Msglogic::ResolveSayHello(base_queue::BlockMsg* block){
	//解析list 并存入
	//一个block 是一个用户行为
	std::string nickname;
	std::stringstream ss;
	std::string summary;
	GIVINGSONGLIST list;
	int64 uid = 0;
	int64 tid = 0;
	ResolveTemplate<msgsvc_logic::GivingSongInfo>(block,uid,tid,list);
	//写入数据库
	ss << nickname  << "给你打招呼";
	summary.assign(ss.str());
	PushMessage(uid,tid,summary);
}

void Msglogic::ResolveGiving(base_queue::BlockMsg* block){
	//解析list 并存入
	//一个block 是一个用户行为
	std::string nickname;
	std::stringstream ss;
	std::string summary;
	std::string conver_num;
	GIVINGSONGLIST list;
	int64 uid = 0;
	int64 tid = 0;
	ResolveTemplate<msgsvc_logic::GivingSongInfo>(block,uid,tid,list);
	int32 infos_size = list.size();
	if(infos_size<=0)
		return;
	//写入数据库
	msgsvc_logic::DBComm::RecordGivingSong(list);
	msgsvc_logic::DBComm::AddUserFriend(uid,tid,nickname);
	bool r = base::BasicUtil::ConverNum(infos_size,conver_num);
	if(!r)
		conver_num.append("0");
	ss << nickname  << "赠送"<<conver_num<<"首歌";
	summary.assign(ss.str());
	PushMessage(uid,tid,summary);
}

void Msglogic::PushMessage(const int64 uid,const int64 tid,const std::string& message){
	//新增消息
	int64 new_msg = 1;
	base_push::PushConnectorEngine::GetPushConnectorEngine()->PushUserMessage(1000,tid,
			message,message,SOUND_TYPE|VIBRATE_TYPE|CLEAR_TYPE,new_msg);
}

template <typename ELEMENT>
void Msglogic::ResolveTemplate(base_queue::BlockMsg* block,int64& uid,int64& tid,
		std::list<ELEMENT>& list){
	std::string name = block->Name();
	int32 msgtype = block->MsgType();
	double distance = 0;
	bool r = block->GetReal(L"distance",&distance);
	if(!r)
		distance = 0;

	base_logic::ListValue* value_list = block->message_list();
	int32 size = value_list->GetSize();
	int32 i = 0;
	while(i < size){
		base_logic::DictionaryValue* message;
		value_list->GetDictionary(i,&message);
		//模板
		ELEMENT element(message);
		element.set_msgtype(msgtype);
		element.set_distance(distance);
		list.push_back(element);
		uid = element.uid();
		tid = element.tid();
		i++;
	}

	//手动进行强制删除
	if(value_list){
		delete value_list;
		value_list = NULL;
	}
}

}

