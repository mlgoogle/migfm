#ifndef _xmppstanzaparser_h_
#define _xmppstanzaparser_h_

#include "xmlparser.h"
#include "xmlbuilder.h"


namespace base {

class XmlElement;

class XmppStanzaParseHandler {
public:
  virtual void StartStream(const XmlElement * pelStream) = 0;
  virtual void Stanza(const XmlElement * pelStanza) = 0;
  virtual void EndStream() = 0;
  virtual void XmlError() = 0;
};

class XmppStanzaParser {
public:
  XmppStanzaParser(XmppStanzaParseHandler *psph);
  bool Parse(const char * data, size_t len, bool isFinal)
    { return parser_.Parse(data, len, isFinal); }
  void Reset();

private:
  class ParseHandler : public XmlParseHandler {
  public:
    ParseHandler(XmppStanzaParser * outer) : outer_(outer) {}
    virtual void StartElement(XmlParseContext * pctx,
               const char * name, const char ** atts)
      { outer_->IncomingStartElement(pctx, name, atts); }
    virtual void EndElement(XmlParseContext * pctx,
               const char * name)
      { outer_->IncomingEndElement(pctx, name); }
    virtual void CharacterData(XmlParseContext * pctx,
               const char * text, int len)
      { outer_->IncomingCharacterData(pctx, text, len); }
    virtual void Error(XmlParseContext * pctx,
               XML_Error errCode)
      { outer_->IncomingError(pctx, errCode); }
  private:
    XmppStanzaParser * const outer_;
  };

  friend class ParseHandler;

  void IncomingStartElement(XmlParseContext * pctx,
               const char * name, const char ** atts);
  void IncomingEndElement(XmlParseContext * pctx,
               const char * name);
  void IncomingCharacterData(XmlParseContext * pctx,
               const char * text, int len);
  void IncomingError(XmlParseContext * pctx,
               XML_Error errCode);

  XmppStanzaParseHandler * psph_;
  ParseHandler innerHandler_;
  XmlParser parser_;
  int depth_;
  XmlBuilder builder_;

 };


}

#endif
