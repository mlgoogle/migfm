#include <expat.h>
#include "xmlelement.h"
#include "xmppstanzaparser.h"
#include "basic/constants.h"

#define new TRACK_NEW

namespace base {

XmppStanzaParser::XmppStanzaParser(XmppStanzaParseHandler *psph) :
  psph_(psph),
  innerHandler_(this),
  parser_(&innerHandler_),
  depth_(0),
  builder_() {
}

void
XmppStanzaParser::Reset() {
  parser_.Reset();
  depth_ = 0;
  builder_.Reset();
}

void
XmppStanzaParser::IncomingStartElement(
    XmlParseContext * pctx, const char * name, const char ** atts) {
	
	if (depth_++ == 0) {
		XmlElement * pelStream = XmlBuilder::BuildElement(pctx, name, atts);
		if (pelStream == NULL) {
		pctx->RaiseError(XML_ERROR_SYNTAX);
		return;
	}
    delete pelStream;
    return;
  }

  builder_.StartElement(pctx, name, atts);
}

void
XmppStanzaParser::IncomingCharacterData(
    XmlParseContext * pctx, const char * text, int len) {
  if (depth_ > 1) {
    builder_.CharacterData(pctx, text, len);
  }
}

void
XmppStanzaParser::IncomingEndElement(
    XmlParseContext * pctx, const char * name) {
  if (--depth_ == 0) {
    psph_->EndStream();
    return;
  }

  builder_.EndElement(pctx, name);

  if (depth_ == 1) {
    XmlElement *element = builder_.CreateElement();
    psph_->Stanza(element);
    delete element;
  }
}

void
XmppStanzaParser::IncomingError(
    XmlParseContext * pctx, XML_Error errCode) {
  //UNUSED(pctx);
  //UNUSED(errCode);
  psph_->XmlError();
}

}

