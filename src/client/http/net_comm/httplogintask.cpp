#include "httplogintask.h"
#include <string>
#include <iostream>

#include "net_comm/http_engine_impl.h"

namespace net_comm{

HttpLoginTask::HttpLoginTask(HttpEngineImpl* pctx)
:pctx_(pctx)
,state_(LOGINSTATE_INIT){

}

HttpLoginTask::~HttpLoginTask(){

}

void
HttpLoginTask::IncomingStanza(bool isStart){
	Advance();
}

bool
HttpLoginTask::Advance(){

	for (;;){

		switch (state_){
			case  LOGINSTATE_INIT:
				pctx_->InternalRequestLogin(pctx_->GetPassword());
				return true;
		}
	}
}




}
