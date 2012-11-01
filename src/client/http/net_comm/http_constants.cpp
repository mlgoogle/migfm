#include "net_comm/http_constants.h"
#include <string>
#include "net_comm/hid.h"
using namespace net_comm;

//const Hid HID_EMPTY(STR_EMPTY);

const std::string & HttpConstants::str_empty(){
	static const std::string str_empty;
	return str_empty;
}

const Hid& HttpConstants::hid_empty(){
	static const Hid hid_emtpy(STR_EMPTY,STR_EMPTY);
	return hid_emtpy;
}