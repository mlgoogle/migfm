// Because global constant initialization order is undefined
// globals cannot depend on other objects to be instantiated.
// This class creates string objects within static methods 
// such that globals may refer to these constants by the
// accessor function and they are guaranteed to be initialized.

#ifndef TALK_XMLLITE_CONSTANTS_H_
#define TALK_XMLLITE_CONSTANTS_H_

#include <string>

#define STR_EMPTY    XmlConstants::str_empty()
#define NS_XML       XmlConstants::ns_xml()
#define NS_XMLNS     XmlConstants::ns_xmlns()
#define STR_XMLNS    XmlConstants::str_xmlns()
#define STR_XML      XmlConstants::str_xml()
#define STR_VERSION  XmlConstants::str_version()
#define STR_ENCODING XmlConstants::str_encoding()
namespace base {
	
class XmlConstants {
 public:
  static const std::string & str_empty();
  static const std::string & ns_xml();
  static const std::string & ns_xmlns();
  static const std::string & str_xmlns();
  static const std::string & str_xml();
  static const std::string & str_version();
  static const std::string & str_encoding();
};

}

#endif  // TALK_XMLLITE_CONSTANTS_H_
