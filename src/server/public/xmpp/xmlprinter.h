#ifndef _xmlprinter_h_
#define _xmlprinter_h_

#include <iosfwd>
#include <string>
#include "basic/scoped_ptr.h"

namespace base {

class XmlElement;

class XmlPrinter {
public:
  static void PrintXml(std::ostream * pout, const XmlElement * pelt);

  static void PrintXml(std::ostream * pout, const XmlElement * pelt,
    const std::string * const xmlns, int xmlnsCount);
};

}

#endif
