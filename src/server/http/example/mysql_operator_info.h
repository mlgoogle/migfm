#ifndef MIG_FM_MYSQL_OPERATOR_INFO_H__
#define MIG_FM_MYSQL_OPERATOR_INFO_H__
#include "storage.h"

enum{
	ADD_USER_INFO = 1,
};

struct UserInfo: public BaseStorage{
	std::string id_; 
	std::string usrname_;
	std::string password_;
	int  sex_;
	int address_;
	std::string name_;
	std::string id_cards_;
	std::string head_;
};

#endif