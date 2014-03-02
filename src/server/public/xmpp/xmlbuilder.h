#ifndef _xmlbuilder_h_
#define _xmlbuilder_h_

#include <string>
#include "basic/scoped_ptr.h"
#include "basic/stl_decl.h"
#include "xmlparser.h"

#include <expat.h>

namespace base {

class XmlElement;
class XmlParseContext;


class XmlBuilder : public XmlParseHandler {
public:
  XmlBuilder();

  static XmlElement * BuildElement(XmlParseContext * pctx,
                                  const char * name, const char ** atts);
  virtual void StartElement(XmlParseContext * pctx,
                            const char * name, const char ** atts);
  virtual void EndElement(XmlParseContext * pctx, const char * name);
  virtual void CharacterData(XmlParseContext * pctx,
                             const char * text, int len);
  virtual void Error(XmlParseContext * pctx, XML_Error);
  virtual ~XmlBuilder();

  void Reset();

  // Take ownership of the built element; second call returns NULL
  XmlElement * CreateElement();

  // Peek at the built element without taking ownership
  XmlElement * BuiltElement();

private:
  XmlElement * pelCurrent_;
  scoped_ptr<XmlElement> pelRoot_;
  scoped_ptr<std::vector<XmlElement *, std::allocator<XmlElement *> > >
    pvParents_;
};

}

#endif
