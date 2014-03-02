#ifndef _MIG_FM_PUBLIC_XMPP_XML_SERIALIZATION_H__
#define _MIG_FM_PUBLIC_XMPP_XML_SERIALIZATION_H__

#include <string>
#include "basic/basictypes.h"

namespace base {

class XmlSerialization{
public:
	XmlSerialization(){}
	virtual ~XmlSerialization(){}
public:
	static bool XmlUserInfoSerialization(std::string& xml_result,const int32 usr_id,
                                         const std::string& username,const int32& sex,
                                         const std::string& extadd,const std::string& street,
                                         const std::string& locality,const std::string& regin,
	                                 const int32& pcode,std::string& ctry,const std::string& head,
	                                 const std::string& birthday,const std::string& nickname);
};


}
#endif
