#include "constants.h"


namespace base{

const std::string& ConStants::str_empty(){
   static const std::string str_empty;
   return str_empty;
}

const std::string & ConStants::ns_session() {
	static const std::string ns_session_("urn:ietf:params:xml:ns:xmpp-session");
	return ns_session_;
}

const std::string & ConStants::ns_bind() {
	static const std::string ns_bind_("urn:ietf:params:xml:ns:xmpp-bind");
	return ns_bind_;
}

const std::string & ConStants::ns_stream() {
	static const std::string ns_stream_("http://etherx.jabber.org/streams");
	return ns_stream_;
}

const QName QN_SESSION_SESSION(true, NS_SESSION, "session");
const QName QN_BIND_BIND(true, NS_BIND, "bind");
const QName QN_STREAM_FEATURES(true, NS_STREAM, "features");
}
