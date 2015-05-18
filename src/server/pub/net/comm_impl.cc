/*
 * comm_impl.cc
 *
 *  Created on: 2014年12月4日
 *      Author: kerry
 */

#include "soc_comm_head.h"

namespace netcomm_recv{


}

namespace netcomm_send{

void HeadPacket::GetJsonSerialize(std::string* json){
	 base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(0,json);
	 base_logic::Value* value = (base_logic::Value*)this->release();
	 engine->Serialize(*value);
}


}


