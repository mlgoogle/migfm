#ifndef _NET_COMM_HTTP_CONSTANTS_H__
#define _NET_COMM_HTTP_CONSTANTS_H__
#include <string>

#define STR_EMPTY  net_comm::HttpConstants::str_empty()
#define HID_EMPTY  net_comm::HttpConstants::hid_empty()

namespace net_comm{
	class Hid;
}
namespace net_comm{

//extern const Hid HID_EMPTY;

class HttpConstants{
public:
	static const std::string & str_empty();
	static const net_comm::Hid & hid_empty();
};

}
#endif