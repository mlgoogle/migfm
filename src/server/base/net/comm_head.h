/*
 * comm_head.h
 *
 *  Created on: 2014年11月17日
 *      Author: kerry
 */

#ifndef _NET_COMM_HEAD_H_
#define _NET_COMM_HEAD_H_

#include "basic/basictypes.h"
#include <list>
#include <string>

namespace netcomm_send{
/*******发送相关*************/
struct BaseSend{
	std::string msg;
	std::string status;
};



//无实体数据返回
struct SendMsg:public BaseSend{

};

/****用户相关****/
//用户注册
struct Regedit:public BaseSend{
	std::string uid;
	std::string username;
	std::string nickname;
	std::string gender;
	int32       type;
	std::string birthday;
	std::string location;
	std::string age;
	std::string head;
	std::string token;
	std::string new_msg_num;
};

//用户登录
struct Login:public BaseSend{
	std::string uid;
	std::string username;
	std::string nickname;
	std::string gender;
	std::string type;
	std::string birthday;
	std::string location;
	std::string age;
	std::string head;
	std::string token;
	std::string new_msg_num;
};


/****聊天相关****/
//获取服务器地址
struct Getsc:public BaseSend{
	std::string host;
	int32       port;
};

struct ChatMsg{
	std::string fid;
	std::string id;
	std::string msg;
	std::string tid;
	std::string time;
};

struct GetHistChat:public BaseSend{
	std::list<struct ChatMsg*> msg_list;
};
/****音乐相关****/
/****社交相关****/
/*******相关****/
}
/*******接收相关************/



#endif /* COMM_HEAD_H_ */
