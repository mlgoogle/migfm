#ifndef _MIG_FM_CONSTANTS_H__
#define _MIG_FM_CONSTANTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "net_comm/xmpp/qname.h"

#define NS_SESSION      base::ConStants::ns_session()
#define NS_BIND			base::ConStants::ns_bind()
#define NS_STREAM       base::ConStants::ns_stream()

namespace base{
	
class ConStants{

public:
    static const std::string& str_empty();
	static const std::string& ns_session();
	static const std::string& ns_bind();
	static const std::string& ns_stream();
};

extern const QName QN_SESSION_SESSION;
extern const QName QN_BIND_BIND;
extern const QName QN_STREAM_FEATURES;
}
#endif
