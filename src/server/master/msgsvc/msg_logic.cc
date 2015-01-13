#include "msg_logic.h"
#include "common.h"

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

    return true;
}



bool Msglogic::OnTimeout(struct server *srv, char *id, int opcode, int time){

    return true;
}

bool Msglogic::GetPushMessageQueue(){
	//读取redis
}

}

