/*
 * response.cc
 *
 *  Created on: 2014年11月17日
 *      Author: kerry
 */
#include "response_result.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"//自动释放




namespace base_net{

bool ResponseResult::BaseManager::OnBaseSend(const int socket,const std::string& msg,const std::string& status,
		base_logic::Value* sub){
	std::string json_str;
	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	dict->Set("msg",CreateStringValue(msg));
	dict->Set("status",CreateStringValue(status));
	dict->Set("result",sub);

	//转化
	base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(0,&json_str);
	engine->Serialize(*dict);
	//发送

}

bool ResponseResult::UserManager::OnUserRegister(const int socket,const std::string& msg,const std::string& status,
		const std::string& uid,const std::string& username,const std::string& nickname,const std::string& gender,
		const int32& type,const std::string& birthday,const std::string& location,const std::string& age,
		const std::string& head,const std::string& token,const std::string& new_msg_num){


	scoped_ptr<base_logic::DictionaryValue> dict(new base_logic::DictionaryValue());
	dict->Set("uid",base_logic::Value::CreateStringValue(uid));
	dict->Set("username",base_logic::Value::CreateStringValue(username));
	dict->Set("nickname",base_logic::Value::CreateStringValue(nickname));
	dict->Set("gender",base_logic::Value::CreateStringValue(gender));
	dict->Set("type",base_logic::Value::CreateCreateIntegerValue(type));
	dict->Set("birthday",base_logic::Value::CreateStringValue(birthday));
	dict->Set("location",base_logic::Value::CreateStringValue(location));
	dict->Set("age",base_logic::Value::CreateStringValue(age));
	dict->Set("head",base_logic::Value::CreateStringValue(head));
	dict->Set("token",base_logic::Value::CreateStringValue(token));
	dict->Set("new_msg_num",base_logic::Value::CreateStringValue(new_msg_num));
	//发送

}

}



