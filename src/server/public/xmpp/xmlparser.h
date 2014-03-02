#ifndef _xmlparser_h_
#define _xmlparser_h_

#include <string>
#include <expat.h>

#include "xmlnsstack.h"

struct XML_ParserStruct;
typedef struct XML_ParserStruct* XML_Parser;

namespace base {

class XmlParseHandler;
class XmlParseContext;
class XmlParser;

class XmlParseContext {
public:
  virtual QName ResolveQName(const char * qname, bool isAttr) = 0;
  virtual void RaiseError(XML_Error err) = 0;
};

class XmlParseHandler {
public:
  virtual void StartElement(XmlParseContext * pctx,
               const char * name, const char ** atts) = 0;
  virtual void EndElement(XmlParseContext * pctx,
               const char * name) = 0;
  virtual void CharacterData(XmlParseContext * pctx,
               const char * text, int len) = 0;
  virtual void Error(XmlParseContext * pctx,
               XML_Error errorCode) = 0;
};

class XmlParser {
public:
  static void ParseXml(XmlParseHandler * pxph, std::string text);

  explicit XmlParser(XmlParseHandler * pxph);
  bool Parse(const char * data, size_t len, bool isFinal);
  void Reset();
  virtual ~XmlParser();

  // expat callbacks
  void ExpatStartElement(const char * name, const char ** atts);
  void ExpatEndElement(const char * name);
  void ExpatCharacterData(const char * text, int len);
  void ExpatXmlDecl(const char * ver, const char * enc, int standalone);

private:

  class ParseContext : public XmlParseContext {
  public:
    ParseContext(XmlParser * parser);
    virtual ~ParseContext();
    virtual QName ResolveQName(const char * qname, bool isAttr);
    virtual void RaiseError(XML_Error err) { if (!raised_) raised_ = err; }
    XML_Error RaisedError() { return raised_; }
    void Reset();

    void StartElement();
    void EndElement();
    void StartNamespace(const char * prefix, const char * ns);

  private:
    const XmlParser * parser_;
    XmlnsStack xmlnsstack_;
    XML_Error raised_;
  };

  ParseContext context_;
  XML_Parser expat_;
  XmlParseHandler * pxph_;
  bool sentError_;
};

}

#endif
