#include "chat_manager.h"
#include "logic_unit.h"
#include "db_comm.h"
#include "dic_comm.h"
#include "base/comm_head.h"
#include "base/protocol.h"
#include "base/logic_comm.h"
#include "config/config.h"
#include "common.h"

namespace chat_logic{


ChatManager::ChatManager(){

	if(!Init())
		assert(0);
}

ChatManager::~ChatManager(){

}



bool ChatManager::Init(){

	bool r = false;
	std::string path = DEFAULT_CONFIG_PATH;
	logic::ThreadKey::InitThreadKey();
	config::FileConfig* config = config::FileConfig::GetFileConfig();
	if(config==NULL){
		return false;
	}

	r = config->LoadConfig(path);
	chat_storage::DBComm::Init(config->mysql_db_list_);
	chat_storage::MemComm::Init(config->mem_list_);

	usr_connection_mgr_.reset(new chat_logic::UserConnectionMgr());
	ims_mgr_.reset(new chat_logic::IMSMgr());
	return true;
}

ChatManager* ChatManager::instance_ = NULL;

ChatManager* 
ChatManager::GetInstance(){
    
	if (instance_==NULL)
		instance_ = new ChatManager;
	return instance_;
}


void ChatManager::FreeInstance(){
	delete instance_;
	instance_ = NULL;
}


bool ChatManager::OnChatConnect(struct server *srv,
								const int socket){
    return true;
}

bool ChatManager::OnChatManagerMessage(struct server *srv,
										 const int socket, 
										 const void *msg, 
										 const int len){
    bool r = false;
	struct PacketHead *packet = NULL;
	if (srv == NULL
		||socket<=0
		||msg == NULL
		||len < PACKET_HEAD_LENGTH)
		return false;

	if (!ProtocolPack::UnpackStream(msg,len,&packet)){
		struct PacketHead *ph = (struct PacketHead*) msg;
		LOG_ERROR2("Call UnpackStream failed.operate_code %d packet_length %d len %d",
			ph->operate_code,ph->packet_length,len);
		return false;
	}

	assert (packet);
	LOG_DEBUG2("packet->packet_length[%d],packet->packet_operate[%d],packet->data_length[%d]",
			packet->packet_length,packet->operate_code,packet->data_length);
	ProtocolPack::HexEncode(msg,len);
	ProtocolPack::DumpPacket(packet);
    
	switch(packet->operate_code){
		case USER_LOGIN:
			{
				usr_connection_mgr_.get()->OnUserLogin(srv,socket,packet);
			}
			break;
		case USER_QUIT:
			{
				usr_connection_mgr_.get()->OnUserQuit(srv,socket,packet);
			}
			break;
		case REQ_OPPOSITION_INFO:
			{
				usr_connection_mgr_.get()->OnGetOppInfos(srv,socket,packet);
			}
			break;
		case TEXT_CHAT_PRIVATE_SEND:
			{
				ims_mgr_.get()->OnMessage(srv,socket,packet);
			}
			break;
		default:
			break;
	}
    return true;
}

bool ChatManager::OnChatManagerClose(struct server *srv,
									 const int socket){
    return true;
}

bool ChatManager::OnBroadcastConnect(struct server *srv,
									 const int socket, 
									 const void *data, 
									 const int len){
    return true;
}

bool ChatManager::OnBroadcastMessage(struct server *srv,
									 const int socket, 
									 const void *msg, 
									 const int len){
    return true;
}

bool ChatManager::OnBroadcastClose(struct server *srv,
									const int socket){
    return true;
}

bool ChatManager::OnIniTimer(struct server *srv){
	return true;
}

bool ChatManager::OnTimeout(struct server *srv, char *id,
							int opcode, int time){
    return true;
}

}
